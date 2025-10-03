#include "XMLValidator.H"
#include "ValidChildren.H"

ValidChildren * XMLValidator::addSchemaElement(std::string element)
{
	std::vector<ValidChildren *>::iterator	schemaElementIterator	= findSchemaElement(element);

	if (schemaElementIterator != schema.end())
	{
		schema.erase(schemaElementIterator);
		delete *schemaElementIterator;
	}

	ValidChildren *	schemaElement	= 0;
	schema.push_back(schemaElement = new ValidChildren(element));
	return schemaElement;
}

std::vector<ValidChildren *>::iterator XMLValidator::findSchemaElement(std::string element)
{
	for (std::vector<ValidChildren *>::iterator i = schema.begin(); i != schema.end(); i++)
		if (((*i)->getThisElement().size() == 0 && element.size() == 0) ||
		  ((*i)->getThisElement().size() > 0 && (*i)->getThisElement().compare(element) == 0))
			return i;

	return schema.end();
}
