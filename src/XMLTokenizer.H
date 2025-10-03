#ifndef XMLTOKENIZER_H
#define XMLTOKENIZER_H

#include <string>
#include <fstream>
#include <ios>
#include <regex>

class XMLTokenizer
{
private:
	std::fstream		file;
	std::string		line;
	int			line_number;
	int			index;
	bool			inside_tag;
	bool			pending_attribute_value;
	bool			tag_found;

	static const char *	PROLOG_START;
	static const char *	PROLOG_IDENTIFIER;
	static const char *	ATTRIBUTE_VALUE;
	static const char *	PROLOG_END;
	static const char *	TAG_START;
	static const char *	ELEMENT;
	static const char *	ATTRIBUTE;
	static const char *	NULL_TAG_END;
	static const char *	TAG_CLOSE_START;
	static const char *	VALUE;
	static const char *	TAG_END;
	static const char *	SPACE_TO_EOL;

	std::regex		prolog_start;
	std::regex		prolog_identifier;
	std::regex		attribute_value;
	std::regex		prolog_end;
	std::regex		tag_start;
	std::regex		element;
	std::regex		attribute;
	std::regex		null_tag_end;
	std::regex		tag_close_start;
	std::regex		value;
	std::regex		tag_end;
	std::regex		space_to_eol;

	virtual void		update_matchers(const std::ssub_match &, const std::ssub_match & suffix);
public:
	class XMLToken
	{
	public:
		enum TokenTypes
		{
			NULL_TOKEN		=  0,
			PROLOG_START		=  1,
			PROLOG_IDENTIFIER	=  2,
			ATTRIBUTE_VALUE		=  3,
			PROLOG_END		=  4,
			TAG_START		=  5,
			ELEMENT			=  6,
			ATTRIBUTE		=  7,
			NULL_TAG_END		=  8,
			TAG_CLOSE_START		=  9,
			VALUE			= 10,
			TAG_END			= 11
		};
	private:
		std::string	token;
		TokenTypes	token_type;
	public:
		XMLToken(const std::string & t, TokenTypes tt);

		virtual ~XMLToken() {}

		virtual const std::string &	getToken(void)		{ return token; }
		virtual TokenTypes		getTokenType(void)	{ return token_type; }
		virtual const char *		toString(void);
	};

	XMLTokenizer(const std::string & filename);
	~XMLTokenizer() {}

	virtual int		getLineNumber(void)	{ return line_number; }
	virtual int		getLineCharacter(void)	{ return index; }
	virtual XMLToken *	getNextToken(void);
};

#endif // XMLTOKENIZER_H
