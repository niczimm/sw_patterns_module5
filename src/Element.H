#ifndef ELEMENT_H
#define ELEMENT_H

#include <string>

#include "Node.H"
#include "NamedNodeMap.H"
#include "ValidChildren.H"
#include "Builder.H"

class XMLTokenizer;

namespace dom
{
	class Attr;
	class NodeList;
	class Document;

	class Element : public virtual Node
	{
	public:
		virtual ~Element() {}

		//
		// Serialization Data Extraction Strategy
		//
		virtual void serialize(std::fstream * writer, WhitespaceStrategy * whitespace)		= 0;

		virtual const std::string &	getAttribute(const std::string & name)					= 0;
		virtual Attr *			getAttributeNode(const std::string & name)				= 0;
		virtual NodeList *		getElementsByTagName(const std::string & tagName)			= 0;
		virtual const std::string &	getTagName(void)							= 0;
		virtual bool			hasAttribute(const std::string & name)					= 0;
		virtual void			removeAttribute(const std::string & name)				= 0;
		virtual Attr *			removeAttributeNode(Attr * oldAttr)					= 0;
		virtual void			setAttribute(const std::string & name, const std::string & value)	= 0;
		virtual Attr *			setAttributeNode(Attr * newAttr)					= 0;

		virtual dom::NamedNodeMap *	getAttributes(void)							= 0;
		virtual bool			hasAttributes(void)							= 0;
	};
};

// Proxy Pattern - Real Subject
class Element_Impl : public virtual dom::Element, Node_Impl
{
private:
	NamedNodeMap_Impl	attributes;
protected:
	Element_Impl(const std::string & tagName, dom::Document * document);
public:
	friend class Document_Impl;

	virtual ~Element_Impl();

	//
	// Serialization Data Extraction Strategy
	//
	virtual void serialize(std::fstream * writer, WhitespaceStrategy * whitespace);

	virtual const std::string &	getAttribute(const std::string & name);
	virtual dom::Attr *		getAttributeNode(const std::string & name);
	virtual dom::NodeList *		getElementsByTagName(const std::string & tagName);
	virtual const std::string &	getTagName(void);
	virtual bool			hasAttribute(const std::string & name);
	virtual void			removeAttribute(const std::string & name);
	virtual dom::Attr *		removeAttributeNode(dom::Attr * oldAttr);
	virtual void			setAttribute(const std::string & name, const std::string & value);
	virtual dom::Attr *		setAttributeNode(dom::Attr * newAttr);

	virtual dom::NamedNodeMap *	getAttributes(void)	{ return &attributes; }
	virtual bool			hasAttributes(void)	{ return attributes.size() > 0; }
};

// Proxy Pattern - Virtual Proxy for Element
class ProxyElement : public virtual dom::Element, public virtual Node_Impl
{
private:
	std::string tagName;
    NamedNodeMap_Impl attributes;   // proxy's own attributes until real element is loaded
    std::istream* file;             // file reference to resume parsing
    std::streampos childrenStartPos; // where the element's children begin
    std::streampos childrenEndPos;   // where the element's children end
    bool childrenLoaded;            // whether children have been parsed
	XMLTokenizer* tokenizer;        // tokenizer for parsing children
	dom::Element* realElement;      // the actual element once loaded (null until loaded)

	void loadChildren(); // parse and load children from file
	void parseChildrenFromFile(); // actually parse children using tokenizer

protected:
	dom::Element* getElement();

public:
	ProxyElement(const std::string& tag, dom::Document * document) : Node_Impl(tag, dom::Node::ELEMENT_NODE), tagName(tag), attributes(document), file(nullptr), childrenStartPos(0), childrenEndPos(0), childrenLoaded(false), tokenizer(nullptr), realElement(nullptr) {
		Node_Impl::document = document;  // Set the document reference like Element_Impl does
	}
	virtual ~ProxyElement();

	// Set file position for lazy loading (to avoid passing in constructor or modifying Document_Impl)
	void setChildrenPosition(std::istream* f, XMLTokenizer* t, std::streampos startPos, std::streampos endPos) {
		file = f;
		tokenizer = t;
		childrenStartPos = startPos;
		childrenEndPos = endPos;
	}
	
	std::streampos getChildrenStartPos() const { return childrenStartPos; }

	//
	// Serialization Data Extraction Strategy
	//
	virtual void serialize(std::fstream * writer, WhitespaceStrategy * whitespace);

	// Known methods from Proxy
	virtual const std::string &	getTagName(void);
	virtual dom::NamedNodeMap *	getAttributes(void);
	virtual bool hasAttributes(void);
	
	// Override getOwnerDocument to ensure it always returns the correct document
	virtual dom::Document *		getOwnerDocument(void) { return Node_Impl::document; }

	// Implemented attribute methods - delegate to attributes
	virtual dom::Attr* getAttributeNode(const std::string& name);
	virtual void setAttribute(const std::string& name, const std::string& value);
	virtual dom::Attr* setAttributeNode(dom::Attr* newAttr);
	virtual dom::NodeList* getElementsByTagName(const std::string& tagName);
	virtual void removeAttribute(const std::string& name);
	virtual dom::Attr* removeAttributeNode(dom::Attr* oldAttr);
	virtual const std::string & getAttribute(const std::string & name); // TODO
	virtual bool hasAttribute(const std::string & name);

	// Override Node methods to delegate to real element once loaded
	virtual dom::NodeList *		getChildNodes(void)
	  { loadChildren(); return realElement ? realElement->getChildNodes() : Node_Impl::getChildNodes(); }
	virtual dom::Node *		getFirstChild(void)
	  { loadChildren(); return realElement ? realElement->getFirstChild() : Node_Impl::getFirstChild(); }
	virtual dom::Node *		getLastChild(void)
	  { loadChildren(); return realElement ? realElement->getLastChild() : Node_Impl::getLastChild(); }
	virtual bool			hasChildNodes(void)
	  { loadChildren(); return realElement ? realElement->hasChildNodes() : Node_Impl::hasChildNodes(); }
	  
	// Override mutable methods to delegate to real element once loaded
	virtual dom::Node *		appendChild(dom::Node * newChild)
	  { loadChildren(); return realElement ? realElement->appendChild(newChild) : Node_Impl::appendChild(newChild); }
	virtual dom::Node *		insertBefore(dom::Node * newChild, dom::Node * refChild)
	  { loadChildren(); return realElement ? realElement->insertBefore(newChild, refChild) : Node_Impl::insertBefore(newChild, refChild); }
	virtual dom::Node *		replaceChild(dom::Node * newChild, dom::Node * oldChild)
	  { loadChildren(); return realElement ? realElement->replaceChild(newChild, oldChild) : Node_Impl::replaceChild(newChild, oldChild); }
	virtual dom::Node *		removeChild(dom::Node * oldChild)
	  { loadChildren(); return realElement ? realElement->removeChild(oldChild) : Node_Impl::removeChild(oldChild); }
};

class ElementDecorator : public virtual dom::Element
{
};

class XMLValidator;
namespace dom
{
	class NodeList;
};

class ElementValidator : public virtual ElementDecorator, public virtual Node_Impl
{
private:
	dom::Element *	parent;
	ValidChildren *	schemaElement;
public:
	ElementValidator(dom::Element * _parent, XMLValidator * xmlValidator);

	//
	// Serialization Data Extraction Strategy
	//
	virtual void serialize(std::fstream * writer, WhitespaceStrategy * whitespace)
	  { parent->serialize(writer, whitespace); }

	//
	// Undecorated pass-through methods.
	//
	virtual const std::string &	getAttribute(const std::string & name)		{ return parent->getAttribute(name); }
	virtual dom::Attr *		getAttributeNode(const std::string & name)	{ return parent->getAttributeNode(name); }
	virtual dom::NodeList *		getElementsByTagName(const std::string & tagName)
	  { return parent->getElementsByTagName(tagName); }
	virtual const std::string &	getTagName(void)				{ return parent->getTagName(); }
	virtual bool			hasAttribute(const std::string & name)		{ return parent->hasAttribute(name); }
	virtual void			removeAttribute(const std::string & name)	{ parent->removeAttribute(name); }
	virtual dom::Attr *		removeAttributeNode(dom::Attr * oldAttr) { return parent->removeAttributeNode(oldAttr); }

	//
	// Decorated Methods
	//
	virtual void			setAttribute(const std::string & name, const std::string & value);
	virtual dom::Attr *		setAttributeNode(dom::Attr * newAttr);
	virtual dom::Node *		insertBefore(dom::Node * newChild, dom::Node * refChild);
	virtual dom::Node *		replaceChild(dom::Node * newChild, dom::Node * oldChild);
	virtual dom::Node *		appendChild(dom::Node * newChild);

	//
	// Reimplemented Node members
	//
	virtual dom::NamedNodeMap *	getAttributes(void)				{ return parent->getAttributes(); }
	virtual bool			hasAttributes(void)				{ return parent->hasAttributes(); }
	virtual const std::string &	getNodeName(void)				{ return parent->getNodeName(); }
	virtual const std::string &	getNodeValue(void)				{ return parent->getNodeValue(); }
	virtual void			setNodeValue(const std::string & nodeValue)	{ parent->setNodeValue(nodeValue); }
	virtual short			getNodeType(void)				{ return parent->getNodeType(); }
	virtual dom::Node *		getParentNode(void)				{ return parent->getParentNode(); }
	virtual dom::NodeList *		getChildNodes(void)				{ return parent->getChildNodes(); }
	virtual dom::Node *		getFirstChild(void)				{ return parent->getFirstChild(); }
	virtual dom::Node *		getLastChild(void)				{ return parent->getLastChild(); }
	virtual dom::Node *		getPreviousSibling(void)			{ return parent->getPreviousSibling(); }
	virtual dom::Node *		getNextSibling(void)				{ return parent->getNextSibling(); }
	virtual dom::Document *		getOwnerDocument(void)				{ return parent->getOwnerDocument(); }
	virtual dom::Node *		removeChild(Node * oldChild)			{ return parent->removeChild(oldChild); }
	virtual bool			hasChildNodes(void)				{ return parent->hasChildNodes(); }
	virtual const std::string &	getLocalName(void)				{ return parent->getLocalName(); }
};

#endif // ELEMENT_H
