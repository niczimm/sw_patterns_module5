#include "Director.H"
#include "Builder.H"

Director::Director(const std::string & filename, Builder * builder)
{
	XMLTokenizer				tokenizer(filename);
	State					documentLocation	= BEFORE_PROLOG;
	XMLTokenizer::XMLToken::TokenTypes	lastToken		= XMLTokenizer::XMLToken::NULL_TOKEN;
	XMLTokenizer::XMLToken::TokenTypes	currentToken		= XMLTokenizer::XMLToken::NULL_TOKEN;
	XMLTokenizer::XMLToken *		token			= 0;

	do
	{
		token		= tokenizer.getNextToken();
		currentToken	= token->getTokenType();

		switch(documentLocation)
		{
		case BEFORE_PROLOG:
			switch(lastToken)
			{
			case XMLTokenizer::XMLToken::NULL_TOKEN:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::PROLOG_START:
					builder->createProlog();
					documentLocation	= AFTER_PROLOG;
					break;
				default:
					break;
				}
			default:
				// Shouldn't be able to get here.
				break;
			}
			break;
		case AFTER_PROLOG:
			switch(lastToken)
			{
			case XMLTokenizer::XMLToken::PROLOG_START:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::PROLOG_IDENTIFIER:
					builder->identifyProlog(token->getToken());
					break;
				default:
					break;
				}
				break;
			case XMLTokenizer::XMLToken::PROLOG_IDENTIFIER:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::ATTRIBUTE:
					builder->createAttribute(token->getToken());
					break;
				case XMLTokenizer::XMLToken::PROLOG_END:
					builder->endProlog();
					documentLocation	= PARSING_ELEMENT;
					break;
				default:
					break;
				}
				break;
			case XMLTokenizer::XMLToken::ATTRIBUTE:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::ATTRIBUTE_VALUE:
					builder->valueAttribute(token->getToken());
					break;
				default:
					break;
				}
				break;
			case XMLTokenizer::XMLToken::ATTRIBUTE_VALUE:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::ATTRIBUTE:
					builder->createAttribute(token->getToken());
					break;
				case XMLTokenizer::XMLToken::PROLOG_END:
					builder->endProlog();
					documentLocation	= PARSING_ELEMENT;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		case PARSING_ELEMENT:
			switch(lastToken)
			{
			case XMLTokenizer::XMLToken::TAG_START:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::ELEMENT:
					builder->createElement(token->getToken());
					break;
				default:
					break;
				}
				break;
			case XMLTokenizer::XMLToken::ELEMENT:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::ATTRIBUTE:
					builder->createAttribute(token->getToken());
					break;
				case XMLTokenizer::XMLToken::TAG_END:
					documentLocation	= IN_NONNULL_ELEMENT;
					builder->pushElement();
					break;
				case XMLTokenizer::XMLToken::NULL_TAG_END:
					break;
				default:
					break;
				}
				break;
			case XMLTokenizer::XMLToken::ATTRIBUTE:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::ATTRIBUTE_VALUE:
					builder->valueAttribute(token->getToken());
					break;
				default:
					break;
				}
				break;
			case XMLTokenizer::XMLToken::ATTRIBUTE_VALUE:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::ATTRIBUTE:
					builder->createAttribute(token->getToken());
					break;
				case XMLTokenizer::XMLToken::TAG_END:
					documentLocation	= IN_NONNULL_ELEMENT;
					builder->pushElement();
					break;
				case XMLTokenizer::XMLToken::NULL_TAG_END:
					break;
				default:
					break;
				}
				break;
			case XMLTokenizer::XMLToken::PROLOG_END:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::TAG_START:
					// Actually create element when we read tag name.
					break;
				default:
					break;
				}
				break;
			case XMLTokenizer::XMLToken::NULL_TAG_END:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::TAG_START:
					// Actually create element when we read tag name.
					break;
				case XMLTokenizer::XMLToken::TAG_CLOSE_START:
					documentLocation	= IN_NONNULL_ELEMENT;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			break;
		case IN_NONNULL_ELEMENT:
			switch(lastToken)
			{
			case XMLTokenizer::XMLToken::ELEMENT:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::TAG_END:
					if (!builder->popElement())
						documentLocation	= END;
					break;
				default:
					break;
				}
				break;
			case XMLTokenizer::XMLToken::TAG_END:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::TAG_START:
					documentLocation	= PARSING_ELEMENT;
					// Actually create element when we read tag name.
					break;
				case XMLTokenizer::XMLToken::VALUE:
					builder->addValue(token->getToken());
					break;
				case XMLTokenizer::XMLToken::TAG_CLOSE_START:
					break;
				default:
					break;
				}
				break;
			case XMLTokenizer::XMLToken::VALUE:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::TAG_CLOSE_START:
					break;
				default:
					break;
				}
			case XMLTokenizer::XMLToken::TAG_CLOSE_START:
				switch(currentToken)
				{
				case XMLTokenizer::XMLToken::ELEMENT:
					builder->confirmElement(token->getToken());
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			break;
		case END:
			switch(currentToken)
			{
			case XMLTokenizer::XMLToken::NULL_TOKEN:
				break;
			default:
				break;
			}
			break;
		default:
			// Shouldn't be able to get here.
			break;
		}

		lastToken	= currentToken;

		if (token != 0)
			delete token;
	} while(currentToken != XMLTokenizer::XMLToken::NULL_TOKEN);

	// Build is complete, reset the builder:
	builder->reset();
}
