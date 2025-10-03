#include <stdio.h>
#include <fstream>
#include <ios>
#include "Attr.H"
#include "Document.H"
#include "Element.H"
#include "Text.H"
#include "XMLTokenizer.H"
#include "XMLSerializer.H"
#include "XMLValidator.H"
#include "Builder.H"
#include "Director.H"

void testTokenizer(int argc, char** argv);
void testSerializer(int argc, char** argv);
void testValidator(int argc, char** argv);
void testIterator(int argc, char** argv);
void testDirector(int argc, char** argv);

void printUsage(void)
{
	printf("Usage:\n");
	printf("\tTest t [file] ...\n");
	printf("\tTest s [file1] [file2]\n");
	printf("\tTest v [file]\n");
	printf("\tTest i\n");
	printf("\tTest d [file1] [file2]\n");
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printUsage();
		exit(0);
	}

	switch(argv[1][0])
	{
	case 'T':
	case 't':
		testTokenizer(argc, argv);
		break;
	case 'S':
	case 's':
		testSerializer(argc, argv);
		break;
	case 'V':
	case 'v':
		testValidator(argc, argv);
		break;
	case 'I':
	case 'i':
		testIterator(argc, argv);
		break;
	case 'D':
	case 'd':
		testDirector(argc, argv);
		break;
	}
}

void testTokenizer(int argc, char** argv)
{
	dom::Document *	document	= Document_Impl::getInstance();

	dom::Element *	element	= document->createElement("NewElement");
	dom::Text *	text	= document->createTextNode("Text Data");
	dom::Attr *	attr	= document->createAttribute("NewAttribute");

	printf("Element Tag = '%s'\n", element->getTagName().c_str());
	printf("Text Data = '%s'\n", text->getValue().c_str());
	printf("Attr Name = '%s'\n", attr->getName().c_str());

	element->setAttributeNode(attr);
	printf("Element attribute '%s'='%s'\n", element->getTagName().c_str(), element->getAttribute("NewAttribute").c_str());

	delete element;
	delete text;
	delete attr;
	delete document;

	for (int i = 2; i < argc; i++)
	{
		XMLTokenizer	tokenizer(argv[i]);

		XMLTokenizer::XMLToken *	token	= 0;

		printf("File:  '%s'\n", argv[i]);

		do
		{
			delete	token;
			token	= tokenizer.getNextToken();

			printf("\tLine %d:  %s = '%s'\n", tokenizer.getLineNumber(),
			  token->toString(), token->getToken().size() == 0 ? "" : token->getToken().c_str());

		} while (token->getTokenType() != XMLTokenizer::XMLToken::NULL_TOKEN);

		delete	token;
	}
}

void testSerializer(int argc, char** argv)
{
	if (argc < 4)
	{
		printUsage();
		exit(0);
	}

	//
	// Create tree of this document:
	// <? xml version="1.0" encoding="UTF-8"?>
	// <document>
	//   <element attribute="attribute value"/>
	//   <element/>
	//   <element attribute="attribute value" attribute2="attribute2 value">
	//     Element Value
	//   </element>
	//   <element>
	//   </element>
	// </document>
	//
	dom::Document *	document	= Document_Impl::getInstance();
	dom::Element *	root		= document->createElement("document");
	document->appendChild(root);

	dom::Element *	child		= document->createElement("element");
	dom::Attr *	attr		= document->createAttribute("attribute");
	attr->setValue("attribute value");
	child->setAttributeNode(attr);
	root->appendChild(child);

	child				= document->createElement("element");
	root->appendChild(child);

	child				= document->createElement("element");
	child->setAttribute("attribute", "attribute value");
	child->setAttribute("attribute2", "attribute2 value");
	dom::Text *	text		= document->createTextNode("Element Value");
	child->appendChild(text);
	root->appendChild(child);

	child				= document->createElement("element");
	root->appendChild(child);

	//
	// Serialize
	//
	std::fstream *	file	= 0;
	XMLSerializer	xmlSerializer(file = new std::fstream(argv[2], std::ios_base::out));
	xmlSerializer.serializePretty(document);
	delete file;
	XMLSerializer	xmlSerializer2(file = new std::fstream(argv[3], std::ios_base::out));
	xmlSerializer2.serializeMinimal(document);
	delete file;

	// delete Document and tree.
}

void testValidator(int argc, char** argv)
{
	if (argc < 3)
	{
		printUsage();
		exit(0);
	}

	//
	// Create tree of this document:
	// <? xml version="1.0" encoding="UTF-8"?>
	// <document>
	//   <element attribute="attribute value"/>
	//   <element/>
	//   <element attribute="attribute value" attribute2="attribute2 value">
	//     Element Value
	//   </element>
	//   <element>
	//   </element>
	// </document>
	//
	// Schema for this document:
	// document contains:  element
	// element contains:  element
	// element contains attributes:  attribute, attribute2
	//
	XMLValidator	xmlValidator;
	ValidChildren *	schemaElement	= xmlValidator.addSchemaElement("");
	schemaElement->addValidChild("document", false);
	schemaElement	= xmlValidator.addSchemaElement("document");
	schemaElement->addValidChild("element", false);
	schemaElement	= xmlValidator.addSchemaElement("element");
	schemaElement->addValidChild("element", false);
	schemaElement->addValidChild("attribute", true);
	schemaElement->addValidChild("attribute2", true);
	schemaElement->setCanHaveText(true);

	dom::Document *	document	= new DocumentValidator(Document_Impl::getInstance(), &xmlValidator);
	dom::Element *	root		= 0;
	dom::Element *	child		= 0;
	dom::Attr *	attr		= 0;

	root		= new ElementValidator(document->createElement("document"), &xmlValidator);
	document->appendChild(root);
	child		= new ElementValidator(document->createElement("element"), &xmlValidator);
	attr		= document->createAttribute("attribute");
	attr->setValue("attribute value");
	child->setAttributeNode(attr);
	root->appendChild(child);
	child		= new ElementValidator(document->createElement("element"), &xmlValidator);
	root->appendChild(child);
	child		= new ElementValidator(document->createElement("element"), &xmlValidator);
	child->setAttribute("attribute", "attribute value");
	child->setAttribute("attribute2", "attribute2 value");
	dom::Text *	text		= document->createTextNode("Element Value");
	child->appendChild(text);
	root->appendChild(child);
	child		= new ElementValidator(document->createElement("element"), &xmlValidator);
	root->appendChild(child);

	//
	// Serialize
	//
	std::fstream *	file	= 0;
	XMLSerializer	xmlSerializer(file = new std::fstream(argv[2], std::ios_base::out));
	xmlSerializer.serializePretty(document);
	delete file;

	// delete Document and tree.
}

void testIterator(int argc, char** argv)
{
	//
	// Create tree of this document:
	// <? xml version="1.0" encoding="UTF-8"?>
	// <document>
	//   <element attribute="attribute value"/>
	//   <element/>
	//   <element attribute="attribute value" attribute2="attribute2 value">
	//     Element Value
	//   </element>
	//   <element>
	//   </element>
	// </document>
	//
	dom::Document *	document	= Document_Impl::getInstance();
	dom::Element *	root		= document->createElement("document");
	document->appendChild(root);
	printf("< 0x%08lx > (Last and highest node out of iterator)\n", (unsigned long )root);

	dom::Element *	child		= document->createElement("element");
	dom::Attr *	attr		= document->createAttribute("attribute");
	attr->setValue("attribute value");
	child->setAttributeNode(attr);
	root->appendChild(child);
	printf("  < 0x%08lx > (First node out of iterator)\n", (unsigned long )child);

	child				= document->createElement("element");
	root->appendChild(child);
	printf("  < 0x%08lx > (Second node out of iterator)\n", (unsigned long )child);

	child				= document->createElement("element");
	child->setAttribute("attribute", "attribute value");
	child->setAttribute("attribute2", "attribute2 value");
	dom::Text *	text		= document->createTextNode("Element Value");
	child->appendChild(text);
	root->appendChild(child);
	printf("  < 0x%08lx > (Fourth node out of iterator)\n", (unsigned long )child);
	printf("    < 0x%08lx > (Third and deepest node out of iterator)\n", (unsigned long )text);

	child				= document->createElement("element");
	root->appendChild(child);
	printf("  < 0x%08lx > (Fifth node out of iterator)\n", (unsigned long )child);

	printf("\nDepth first iteration:\n");
	dom::Iterator *	domIterator;
	for (domIterator = document->createIterator(0); domIterator->hasNext();)
		printf("node:  0x%08lx\n", (unsigned long )domIterator->next());
	delete domIterator;

	// delete Document and tree.
}

void testDirector(int argc, char** argv)
{
	Builder*		builder = Builder::getInstance();
	Director	director(argv[2], builder);
	std::fstream	file(argv[3], std::ios_base::out);
	XMLSerializer	xmlSerializer(&file);
	xmlSerializer.serializePretty(builder->getDocument());
}
