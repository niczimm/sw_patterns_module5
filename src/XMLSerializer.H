#include <string>
#include <fstream>
#include <ios>

#include "Node.H"

//
// XMLSerializer is Serialization Strategy Context
// Node is Serialization Strategy and output stream Strategy Context
// Document, Element, Attr, Text are Serialization Concrete Strategies
// java.io.Writer is output stream Strategy
// java.io.BufferedWriter is output stream Concrete Strategy
//
class XMLSerializer
{
private:
	std::fstream *		file;
public:
	XMLSerializer(std::fstream * _file) : file(_file)	{}
	~XMLSerializer() {}
private:
	class PrettyWhitespaceStrategy : public WhitespaceStrategy
	{
	private:
		int	indentationLevel;
	public:
		PrettyWhitespaceStrategy(void) : indentationLevel(0)	{}

		virtual void	prettyIndentation(std::fstream * wwriter)
		{
			for (int i = 0; i < indentationLevel; i++)
				*wwriter << "\t";
		}

		virtual void incrementIndentation(void)		{ indentationLevel++; }
		virtual void decrementIndentation(void)		{ indentationLevel--; }
		virtual void newLine(std::fstream * wwriter)	{ *wwriter << "\n"; }
	};

	class MinimalWhitespaceStrategy : public WhitespaceStrategy
	{
	public:
		virtual void prettyIndentation(std::fstream * wwriter)	{}
		virtual void incrementIndentation(void)			{}
		virtual void decrementIndentation(void)			{}
		virtual void newLine(std::fstream * wwriter)		{}
	};
public:
	//
	// Strategized serialization
	//
	virtual void		serializePretty(dom::Node * node);
	virtual void		serializeMinimal(dom::Node * node);
};
