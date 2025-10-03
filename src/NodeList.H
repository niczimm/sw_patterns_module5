#ifndef NODELIST_H
#define NODELIST_H

#include <list>

namespace dom
{
	class Node;

	class NodeList : public std::list<Node *>
	{
	public:
		virtual iterator	find(Node *n)
		{
			iterator	i;

			for (i = begin(); i != end(); i++)
				if (*i.operator->() == n)
					break;

			return i;
		}
	};
};

#endif // NODELIST_H
