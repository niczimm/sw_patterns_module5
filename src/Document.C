#include "Document.H"
#include "Element.H"
#include "Text.H"
#include "Attr.H"
#include "NodeList.H"
#include "XMLValidator.H"

Document_Impl::Document_Impl(void) : Node_Impl("", dom::Node::DOCUMENT_NODE)
{
	Node_Impl::document	= this;
}

Document_Impl::~Document_Impl() {}

void Document_Impl::serialize(std::fstream * writer, WhitespaceStrategy * whitespace)
{
	*writer << "<? xml version=\"1.0\" encoding=\"UTF-8\"?>";
	whitespace->newLine(writer);
	getDocumentElement()->serialize(writer, whitespace);
}

dom::Element *	Document_Impl::createElement(const std::string & tagName)
{
	return new Element_Impl(tagName, this);
}

dom::Text *	Document_Impl::createTextNode(const std::string & data)
{
	return new Text_Impl(data, this);
}

dom::Attr *	Document_Impl::createAttribute(const std::string & name)
{
	return new Attr_Impl(name, this);
}

dom::Element * Document_Impl::getDocumentElement()
{
	for (dom::NodeList::iterator i = getChildNodes()->begin(); i != getChildNodes()->end(); i++)
		if (dynamic_cast<dom::Element *>(*i.operator->()) != 0)
			return dynamic_cast<dom::Element *>(*i.operator->());

	return 0;
}

dom::Iterator * Document_Impl::createIterator(dom::Node * node)
{
	return new DOMIterator(node, this);
}

DocumentValidator::DocumentValidator(dom::Document * _parent, XMLValidator * xmlValidator) :
  Node_Impl("", dom::Node::DOCUMENT_NODE),
  parent(_parent)
{
	schemaElement	= *xmlValidator->findSchemaElement("");
}

dom::Node * DocumentValidator::insertBefore(dom::Node * newChild, dom::Node * refChild)
{
	if (schemaElement == 0 || schemaElement->childIsValid(newChild->getNodeName(), false))
		return parent->insertBefore(newChild, refChild);
	else
		throw dom::DOMException(dom::DOMException::VALIDATION_ERR, "Invalid root node " + newChild->getNodeName() + ".");
}

dom::Node * DocumentValidator::replaceChild(dom::Node * newChild, dom::Node * oldChild)
{
	if (schemaElement == 0 || schemaElement->childIsValid(newChild->getNodeName(), false))
		return parent->replaceChild(newChild, oldChild);
	else
		throw dom::DOMException(dom::DOMException::VALIDATION_ERR, "Invalid root node " + newChild->getNodeName() + ".");
}

dom::Node * DocumentValidator::appendChild(dom::Node * newChild)
{
	if (schemaElement == 0 || schemaElement->childIsValid(newChild->getNodeName(), false))
		return parent->appendChild(newChild);
	else
		throw dom::DOMException(dom::DOMException::VALIDATION_ERR, "Invalid root node " + newChild->getNodeName() + ".");
}

DOMIterator::DOMIterator(dom::Node * startWithNode, Document_Impl * document) :
  firstNode(startWithNode == 0 ? (dom::Node * )document->getDocumentElement() : startWithNode)
{
	if (firstNode != 0)
		for (dom::Node * node = firstNode; node->getChildNodes()->size() >0; node = *node->getChildNodes()->begin())
		{
			listStack.push(node->getChildNodes());
			indexStack.push(0);
		}
}

dom::Node * DOMIterator::elementAt(dom::NodeList * currentList, int currentIndex)
{
	int			i;
	dom::NodeList::iterator	it;

	for (i = 0, it = currentList->begin(); it != currentList->end() && i < currentIndex; i++, it++);

	return *it;
}

bool DOMIterator::hasNext()
{
	return firstNode != 0;
}

dom::Node * DOMIterator::next()
{
	dom::NodeList *	currentList	= listStack.size() > 0 ? listStack.top() : 0;

	if (currentList == 0)
	{
		dom::Node *	temp	= firstNode;
		firstNode		= 0;
		return temp;
	}
	else
	{
		int		currentIndex	= indexStack.top();
		indexStack.pop();
		dom::Node *	temp		= elementAt(currentList, currentIndex++);

		if (currentIndex >= currentList->size())
			listStack.pop();
		else
		{
			indexStack.push(currentIndex);

			for (dom::Node * node = elementAt(currentList, currentIndex);
			  node->getChildNodes()->size() > 0;
			  node = *node->getChildNodes()->begin())
			{
				listStack.push(node->getChildNodes());
				indexStack.push(0);
			}
		}

		return temp;
	}
}
