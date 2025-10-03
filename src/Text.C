#include "Text.H"
#include <stdexcept>

Text_Impl::Text_Impl(const std::string value, dom::Document * document) : Node_Impl("", dom::Node::TEXT_NODE)
{
	setNodeValue(value);
	Node_Impl::document	= document;
}

Text_Impl::~Text_Impl()
{
}

void Text_Impl::serialize(std::fstream * writer, WhitespaceStrategy * whitespace)
{
	whitespace->prettyIndentation(writer);
	*writer << getData();
	whitespace->newLine(writer);
}

const std::string &	Text_Impl::getName(void)
{
	return getNodeName();
}

const std::string &	Text_Impl::getData(void)
{
	return getNodeValue();
}

const std::string &	Text_Impl::getValue(void)
{
	return getData();
}

void			Text_Impl::setData(const std::string & value)
{
	setNodeValue(value);
}

void			Text_Impl::setValue(const std::string & value)
{
	setNodeValue(value);
}

int			Text_Impl::getLength(void)
{
	return getValue().size();
}

const std::string &	Text_Impl::substringData(int offset, int count)
{
	try
	{
		//
		// This is not thread-safe, of course.
		//
		static std::string	value;
		value	= getValue().substr(offset, count);

		return value;
	}
	catch (std::out_of_range &)
	{
		throw dom::DOMException(dom::DOMException::INDEX_SIZE_ERR, "Index larger than Text node's value.");
	}
}

void			Text_Impl::appendData(const std::string & arg)
{
	std::string	value	= getValue();

	setValue(value.append(arg));
}

void			Text_Impl::insertData(int offset, const std::string & arg)
{
	std::string	value	= getValue();

	setValue(value.insert(offset, arg));
}

void			Text_Impl::deleteData(int offset, int count)
{
	std::string	value	= getValue();

	setValue(value.erase(offset, count));
}

void			Text_Impl::replaceData(int offset, int count, const std::string & arg)
{
	std::string	value	= getValue();

	setValue(value.erase(offset, count).insert(offset, arg));
}

dom::Text *		Text_Impl::splitText(int offset)
{
	try
	{
		dom::Text *	text	= new Text_Impl(substringData(offset, getLength() - offset), document);

		setValue(substringData(0, offset));

		if (getParentNode() != 0)
			insertBefore(text, getNextSibling());

		return text;
	}
	catch (std::out_of_range &)
	{
		throw dom::DOMException(dom::DOMException::INDEX_SIZE_ERR, "Index larget than Text node's value.");
	}
}
