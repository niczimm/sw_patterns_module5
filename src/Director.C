#include "Director.H"
#include "Builder.H"
#include <cstdio>
#include <regex>
#include <memory>

// Subbed in my director implementation since professor's wasn't working for me
Director::Director(const std::string & filename, Builder * builder)
{
	try {
		XMLTokenizer				tokenizer(filename);
		
		// Pass the file and tokenizer to the builder for proxy pattern
		builder->setFileAndTokenizer(tokenizer.getFileStream(), &tokenizer); // todo- check this with RAII
		
		bool skipProlog = false;

	while (true) {
		// Idiom - RAII with unique_ptr to ensure token is deleted
		// Note: Using unique_ptr here to manage XMLToken memory automatically
		// to prevent memory leaks in case of exceptions
		// Also reusing the same unique_ptr to avoid multiple allocations
		// in a loop, which can be less efficient
		// since getNextToken() returns a new XMLToken each time
		// and we need to delete the previous one before getting the next
		// This way we ensure only one XMLToken exists at a time
		// and it is properly cleaned up
		// This is a common C++ idiom for resource management
		// and exception safety
		std::unique_ptr<XMLTokenizer::XMLToken> token(tokenizer.getNextToken());

		if (token->getTokenType() == XMLTokenizer::XMLToken::NULL_TOKEN)
		{
			break;
		}

		if (skipProlog) {
			if (token->getTokenType() == XMLTokenizer::XMLToken::TAG_END) {
				skipProlog = false;
			}
			continue;
		}	

		switch (token->getTokenType()) {
			case XMLTokenizer::XMLToken::PROLOG_START:
				builder->createProlog();
				skipProlog = true;
				break;

			case XMLTokenizer::XMLToken::TAG_START:
				// Reuse same unique_ptr for next token
				token.reset(tokenizer.getNextToken());
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

				token.reset(tokenizer.getNextToken());
				if (token->getTokenType() == XMLTokenizer::XMLToken::ATTRIBUTE_VALUE) {
					std::string value = token->getToken();
					// Remove quotes
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
				token.reset(tokenizer.getNextToken());
				if (token->getTokenType() == XMLTokenizer::XMLToken::ELEMENT) {
					builder->confirmElement(token->getToken());
				}

				token.reset(tokenizer.getNextToken());
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
