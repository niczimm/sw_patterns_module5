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
	elementStack.top()->appendChild(static_cast<dom::Node *>(factory->createTextNode(trim(text))));
}

void Builder::confirmElement(const std::string & tag)
{
	// Throw an exception if trim(tag) != currentElement.getTagName()
}

void Builder::createAttribute(const std::string & attribute)
{
	std::string	trimmed	= trim(attribute);
	currentAttr	= factory->createAttribute(std::string(trimmed, 0, trimmed.size() - 1));
}

void Builder::createElement(const std::string & tag)
{
	currentElement = factory->createProxyElement(trim(tag));  // Use ProxyElement for lazy loading
	
	// Record the current file position as the start of potential children
	if (xmlFile != nullptr) {
		std::streampos currentPos = xmlFile->tellg();
		ProxyElement* proxy = dynamic_cast<ProxyElement*>(currentElement);
		if (proxy) {
			// Set a preliminary position - we'll update the end position later
			proxy->setChildrenPosition(xmlFile, tokenizer, currentPos, currentPos);
		}
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
		ProxyElement* proxy = dynamic_cast<ProxyElement*>(currentElement);
		if (proxy) {
			std::streampos currentPos = xmlFile->tellg();
			// Update with the current position as the end of children area
			proxy->setChildrenPosition(xmlFile, tokenizer, proxy->getChildrenStartPos(), currentPos);
		}
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
        ProxyElement* proxy = dynamic_cast<ProxyElement*>(currentElement);
        if (proxy && xmlFile != nullptr && tokenizer != nullptr) {
            proxy->setChildrenPosition(xmlFile, tokenizer, startPos, endPos);
        }
    }
}
