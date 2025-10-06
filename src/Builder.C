#include "Builder.H"
#include <iostream>

#include <ctype.h>
#include "Document.H"
#include "Element.H"
#include "Attr.H"
#include "Text.H"

// Forward declaration for ProxyElement
class ProxyElement;

void Builder::addValue(const std::string & text)
{
	// Use the same document that created the current element to avoid document mismatch
	dom::Element* currentElem = elementStack.top();
	dom::Document* elemDoc = currentElem->getOwnerDocument();
	dom::Text* textNode = elemDoc->createTextNode(trim(text));
	currentElem->appendChild(static_cast<dom::Node *>(textNode));
}

void Builder::confirmElement(const std::string & tag)
{
	// Throw an exception if trim(tag) != currentElement.getTagName()
}

void Builder::createAttribute(const std::string & attribute)
{
	std::string	trimmed	= trim(attribute);
	// Use the same document that created the current element
	dom::Document* elemDoc = currentElement ? currentElement->getOwnerDocument() : factory;
	currentAttr	= elemDoc->createAttribute(trimmed);  // Don't truncate the attribute name
}

void Builder::createElement(const std::string & tag)
{
	currentElement = factory->createElement(trim(tag));  // Uses ProxyElement for lazy loading
	
	// Record the current file position as the start of potential children
	if (xmlFile != nullptr) {
		std::streampos currentPos = xmlFile->tellg();
		currentElement->setChildrenPosition(xmlFile, tokenizer, currentPos, currentPos);
	}
	
	if (elementStack.size() == 0)
		factory->appendChild(currentElement);
	else
		elementStack.top()->appendChild(currentElement);
}

void Builder::createProlog(void)
{
	// null method in this implementation
}

void Builder::endProlog(void)
{
	// null method in this implementation
}

void Builder::identifyProlog(const std::string & id)
{
	// null method in this implementation
}

bool Builder::popElement(void)
{
	currentElement	= elementStack.top();
	elementStack.pop();
	return elementStack.size() > 0;
}

void Builder::pushElement(void)
{
	// Before pushing, update the children end position for the current element
	if (currentElement != nullptr && xmlFile != nullptr) {
		std::streampos currentPos = xmlFile->tellg();
		// Update with the current position as the end of children area
		currentElement->setChildrenPosition(xmlFile, tokenizer, currentElement->getChildrenStartPos(), currentPos);
	}
	
	elementStack.push(currentElement);
	currentElement	= 0;
}

void Builder::valueAttribute(const std::string & value)
{
	std::string	trimmed	= trim(value);
	currentAttr->setValue(std::string(trimmed, 1, trimmed.size() - 2));

	if (currentElement != 0)	// Discard prolog attributes.  This implementation currently doesn't have
					// anything to do with them.
		currentElement->setAttributeNode(currentAttr);
}

const std::string Builder::trim(const std::string & s) const
{
	int	start_index;
	int	stop_index;

	for (start_index = 0; start_index < s.size() && isspace(s[start_index]); start_index++);
	for (stop_index = s.size() - 1; stop_index >= start_index && isspace(s[stop_index]); stop_index--);

	return std::string(s, start_index, stop_index - start_index + 1);
}

void Builder::reset() {
    elementStack = {};
    currentElement = nullptr;
    currentAttr = nullptr;
}

void Builder::setCurrentElementLazyInfo(std::streampos startPos, std::streampos endPos) {
    if (currentElement != nullptr) {
        if (xmlFile != nullptr && tokenizer != nullptr) {
            currentElement->setChildrenPosition(xmlFile, tokenizer, startPos, endPos);
        }
    }
}
