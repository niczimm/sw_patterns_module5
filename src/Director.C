#include "Director.H"
#include "Builder.H"
#include <cstdio>
#include <regex>

// Subbed in my director implementation since professor's wasn't working for me
Director::Director(const std::string & filename, Builder * builder)
{
	try {
		XMLTokenizer				tokenizer(filename);
		
		// Pass the file and tokenizer to the builder for proxy pattern
		builder->setFileAndTokenizer(tokenizer.getFileStream(), &tokenizer);
		
		XMLTokenizer::XMLToken* token = nullptr;
	bool skipProlog = false;

	while ((token = tokenizer.getNextToken())->getTokenType() != XMLTokenizer::XMLToken::NULL_TOKEN) {

	if (skipProlog) {
	if (token->getTokenType() == XMLTokenizer::XMLToken::TAG_END) {
	skipProlog = false;
	}
	delete token;
	continue;
	}

	switch (token->getTokenType()) {
	case XMLTokenizer::XMLToken::PROLOG_START:
	builder->createProlog();
	skipProlog = true;
	break;

	case XMLTokenizer::XMLToken::TAG_START:
	delete token;
	token = tokenizer.getNextToken();
	if (token->getTokenType() == XMLTokenizer::XMLToken::ELEMENT) {
	builder->createElement(token->getToken());
	}
	break;

	case XMLTokenizer::XMLToken::ATTRIBUTE:
	{
	std::string attrToken = token->getToken();
	std::string name = attrToken;
	
	// Handle attribute tokens that may include "=" - extract just the name part
	size_t eqPos = attrToken.find("=");
	if (eqPos != std::string::npos) {
		name = attrToken.substr(0, eqPos);
	}
	
	// Remove leading/trailing whitespace from the name
	name = std::regex_replace(name, std::regex("^\\s+|\\s+$"), "");

	builder->createAttribute(name);

	delete token;
	token = tokenizer.getNextToken();
	if (token->getTokenType() == XMLTokenizer::XMLToken::ATTRIBUTE_VALUE) {
	std::string value = token->getToken();
	// Simple quote removal
	if (value.length() >= 2 && (value[0] == '"' || value[0] == '\'') && 
		(value[value.length()-1] == '"' || value[value.length()-1] == '\'')) {
		value = value.substr(1, value.length() - 2);
	}
	builder->valueAttribute(value);
	}
	}
	break;

	case XMLTokenizer::XMLToken::TAG_END:
	builder->pushElement();
	break;

	case XMLTokenizer::XMLToken::VALUE:
	if (!token->getToken().empty()) {
	std::string value = token->getToken();
	value = std::regex_replace(value, std::regex("^\\s+|\\s+$"), "");
	if (!value.empty()) {
	builder->addValue(value);
	}
	}
	break;

	case XMLTokenizer::XMLToken::TAG_CLOSE_START:
	delete token;
	token = tokenizer.getNextToken();
	if (token->getTokenType() == XMLTokenizer::XMLToken::ELEMENT) {
	builder->confirmElement(token->getToken());
	}
	delete token;
	token = tokenizer.getNextToken();
	if (token->getTokenType() == XMLTokenizer::XMLToken::TAG_END) {
	builder->popElement();
	}
	break;

	case XMLTokenizer::XMLToken::NULL_TAG_END:
	builder->pushElement();
	builder->popElement();
	break;

	default:
	break;
	}

	if (token != nullptr) {
	delete token;
	}
	}

	if (token != nullptr) {
	delete token;
	}

	// Build is complete, reset the builder:
	builder->reset();
	
	} catch (dom::DOMException& e) {
		printf("DOMException caught in Director: Reason=%d, Description=%s\n", 
		       e.getReason(), e.getDescription().c_str());
		throw; // Re-throw to let caller handle
	} catch (const std::exception& e) {
		printf("Exception caught in Director: %s\n", e.what());
		throw; // Re-throw to let caller handle
	} catch (...) {
		printf("Unknown exception caught in Director\n");
		throw; // Re-throw to let caller handle
	}
}
