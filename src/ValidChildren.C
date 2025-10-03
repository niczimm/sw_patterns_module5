#include "ValidChildren.H"

void ValidChildren::addValidChild(const std::string & child, bool isAttribute)
{
	if (childIsValid(child, isAttribute))
		return;

	validChildren.push_back(child);
	childIsAttribute.push_back(isAttribute);
}

bool ValidChildren::childIsValid(const std::string & child, bool isAttribute)
{
	for (int i = 0; i < validChildren.size(); i++)
		if (childIsAttribute[i] == isAttribute && validChildren[i].compare(child) == 0)
			return true;

	return false;
}
