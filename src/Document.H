#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <string>
#include <fstream>
#include <stack>

#include "Node.H"
#include "ValidChildren.H"

namespace dom
{
	class Element;
	class Text;
	class Attr;
	class NodeList;

	class Iterator
	{
	public:
		virtual bool	hasNext()	= 0;
		virtual Node *	next()		= 0;
		virtual ~Iterator() {}
	};

	class Document : public virtual Node
	{
	public:
		//
		// Serialization Data Extraction Strategy
		//
		virtual void serialize(std::fstream * writer, WhitespaceStrategy * whitespace)	= 0;

		virtual Element *	createElement(const std::string & tagName)	= 0;
		virtual Text *		createTextNode(const std::string & data)	= 0;
		virtual Attr *		createAttribute(const std::string & name)	= 0;
		virtual Element *	getDocumentElement()				= 0;

		//
		// Iterator Factory
		//
		virtual Iterator *	createIterator(Node * node)			= 0;
	};
};

class Document_Impl : public virtual dom::Document, public virtual Node_Impl
{
public:
	Document_Impl(void);

	virtual ~Document_Impl();

	//
	// Serialization Data Extraction Strategy
	//
	virtual void serialize(std::fstream * writer, WhitespaceStrategy * whitespace);

	virtual dom::Element *	createElement(const std::string & tagName);
	virtual dom::Text *	createTextNode(const std::string & data);
	virtual dom::Attr *	createAttribute(const std::string & name);
	virtual dom::Element *	getDocumentElement();

	//
	// Iterator Factory
	//
	dom::Iterator *		createIterator(dom::Node * node);
};

class DOMIterator : public virtual dom::Iterator
{
private:
	dom::Node *			firstNode;
	std::stack<dom::NodeList *>	listStack;
	std::stack<int>			indexStack;

	friend class Document_Impl;

	DOMIterator(dom::Node * startWithNode, Document_Impl * document);
	dom::Node * elementAt(dom::NodeList * currentList, int currentIndex);
public:
	virtual bool		hasNext();
	virtual dom::Node *	next();
};

class DocumentDecorator : public virtual dom::Document
{
};

class XMLValidator;

class DocumentValidator : public virtual DocumentDecorator, public virtual Node_Impl
{
private:
	dom::Document *	parent;
	ValidChildren *	schemaElement;
public:
	DocumentValidator(dom::Document * _parent, XMLValidator * xmlValidator);

	//
	// Serialization Data Extraction Strategy
	//
	virtual void serialize(std::fstream * writer, WhitespaceStrategy * whitespace)
	  { parent->serialize(writer, whitespace); }

	//
	// Undecorated pass-through methods.
	//
	virtual dom::Element *	createElement(const std::string & tagName)	{ return parent->createElement(tagName); }
	virtual dom::Text *	createTextNode(const std::string & data)	{ return parent->createTextNode(data); }
	virtual dom::Attr *	createAttribute(const std::string & name)	{ return parent->createAttribute(name); }
	virtual dom::Element *	getDocumentElement()				{ return parent->getDocumentElement(); }

	//
	// Decorated Methods
	//
	virtual dom::Node *	insertBefore(dom::Node * newChild, dom::Node * refChild);
	virtual dom::Node *	replaceChild(dom::Node * newChild, dom::Node * oldChild);
	virtual dom::Node *	appendChild(dom::Node * newChild);

	//
	// Iterator Factory
	//
	dom::Iterator *		createIterator(dom::Node * node)		{ return parent->createIterator(node); }
};

#endif // DOCUMENT_H
