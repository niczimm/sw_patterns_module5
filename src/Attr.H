#ifndef ATTR_H
#define ATTR_H

#include <string>
#include <fstream>

#include "Node.H"

namespace dom
{
	class Document;
	class Element;

	class Attr : public virtual Node
	{
	public:
		virtual ~Attr() {}

		//
		// Serialization Data Extraction Strategy
		//
		virtual void serialize(std::fstream * writer, WhitespaceStrategy * whitespace)	= 0;

		virtual const std::string &	getName(void)				= 0;
		virtual const std::string &	getValue(void)				= 0;
		virtual void			setValue(const std::string & value)	= 0;
		virtual Element *		getOwnerElement(void)			= 0;
	};
};

class Attr_Impl : public virtual dom::Attr, Node_Impl
{
protected:
	Attr_Impl(const std::string & tagName, dom::Document * document);
	Attr_Impl(const std::string & tagName, const std::string & value, dom::Document * document);
public:
	friend class Document_Impl;
	friend class Element_Impl;

	virtual ~Attr_Impl();

	//
	// Serialization Data Extraction Strategy
	//
	virtual void serialize(std::fstream * writer, WhitespaceStrategy * whitespace);

	virtual const std::string &	getName(void);
	virtual const std::string &	getValue(void);
	virtual void			setValue(const std::string & value);
	virtual dom::Element *		getOwnerElement(void);

	//
	// Overridden functions for conforming to Composite Leaf node behavior.
	//
	virtual Node *			insertBefore(Node * newChild, Node * refChild)	{ return 0; }
	virtual Node *			replaceChild(Node * newChild, Node * oldChild)	{ return 0; }
	virtual Node *			removeChild(Node * oldChild)			{ return 0; }
	virtual Node *			appendChild(Node * newChild)			{ return 0; }
};

#endif // ATTR_H
