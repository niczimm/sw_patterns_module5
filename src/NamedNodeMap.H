#ifndef NAMEDNODEMAP_H
#define NAMEDNODEMAP_H

#include <string>

#include "NodeList.H"

namespace dom
{
	class Document;

	class NamedNodeMap : public NodeList
	{
	public:
		virtual Node *	getNamedItem(const std::string & name)		{ return 0; }
		virtual Node *	setNamedItem(Node * arg)			{ return 0; }
		virtual Node *	removeNamedItem(const std::string & name)	{ return 0; }
	};
};

class NamedNodeMap_Impl : public dom::NamedNodeMap
{
private:
	dom::Document *	document;
public:
	NamedNodeMap_Impl(dom::Document * document);

	virtual ~NamedNodeMap_Impl();

	virtual dom::Node *	getNamedItem(const std::string & name);
	virtual dom::Node *	setNamedItem(dom::Node * arg);
	virtual dom::Node *	removeNamedItem(const std::string & name);
};

#endif // NAMEDNODEMAP_H
