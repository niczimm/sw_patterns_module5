#include <string>
#include <fstream>
#include <ios>
#include <vector>

#include "Element.H"

class ValidChildren;

class XMLValidator
{
private:
	std::vector<ValidChildren *>	schema;
public:
	XMLValidator(void)	{}
	~XMLValidator() { for (int i = 0; i < schema.size(); i++) delete schema[i]; }

	virtual ValidChildren *				addSchemaElement(std::string element);
	virtual std::vector<ValidChildren *>::iterator	findSchemaElement(std::string element);
};
