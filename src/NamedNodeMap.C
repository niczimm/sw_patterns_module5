#include "NamedNodeMap.H"
#include "Attr.H"
#include "Element.H"

NamedNodeMap_Impl::NamedNodeMap_Impl(dom::Document * document) : document(0)
{
}

NamedNodeMap_Impl::~NamedNodeMap_Impl()
{
}

dom::Node *	NamedNodeMap_Impl::getNamedItem(const std::string & name)
{
	for (iterator i = begin(); i != end(); i++)
		if ((*i.operator->())->getNodeName().compare(name) == 0)
			return *i.operator->();

	return 0;
}

dom::Node *	NamedNodeMap_Impl::setNamedItem(dom::Node * arg)
{
	if (arg->getOwnerDocument() != document)
		throw dom::DOMException(dom::DOMException::WRONG_DOCUMENT_ERR, "Arg not created by this document.");

	dom::Attr *	attribute;

	if ((attribute = dynamic_cast<dom::Attr *>(arg)) != 0)
	{
		dom::Node *	parent	= attribute->getParentNode();

		if (parent != 0 && dynamic_cast<dom::Element *>(parent) != 0)
			throw dom::DOMException(dom::DOMException::INUSE_ATTRIBUTE_ERR, "Arg not created by this document.");
	}

	iterator	i;

	for (i = begin(); i != end(); i++)
		if ((*i.operator->())->getNodeName().compare(arg->getNodeName()) == 0)
			break;

	if (i != end())
		erase(i);

	push_back(arg);

	return *i.operator->();
}

dom::Node *	NamedNodeMap_Impl::removeNamedItem(const std::string & name)
{
	for (iterator i = begin(); i != end(); i++)
		if ((*i.operator->())->getNodeName().compare(name) == 0)
			return *i.operator->();

	throw dom::DOMException(dom::DOMException::NOT_FOUND_ERR, "Node not found.");
}
