#ifndef BUILDER_H
#define BUILDER_H

#include <stack>
#include <string>
#include "Document.H"

namespace dom
{
	class Document;
	class Element;
	class Attr;
};

// Singleton
class Builder
{
private:
	dom::Document *			factory;
	std::stack<dom::Element *>	elementStack;
	dom::Element *			currentElement;
	dom::Attr *			currentAttr;

	virtual const std::string	trim(const std::string & s) const;

	// Singleton - Private constructor so clients cannot create more than one instance
	Builder() : factory(Document_Impl::getInstance()), currentElement(0), currentAttr(0) {}

public:

	// Singleton - Delete copy constructor and assignment
    Builder(const Builder&) = delete;
    Builder& operator=(const Builder&) = delete;

	// Singleton - Create instance if it doesn't exist, return the instance
    static Builder* getInstance() {
		static Builder* instance = nullptr; // created on first use
		if (instance == nullptr) {
			instance = new Builder();
		}
        return instance;
    }

	virtual dom::Document *	getDocument(void)	{ return factory; }	// aka getResult

	virtual void		addValue(const std::string & text);
	virtual void		confirmElement(const std::string & tag);
	virtual void		createAttribute(const std::string & attribute);
	virtual void		createElement(const std::string & tag);
	virtual void		createProlog(void);
	virtual void		endProlog(void);
	virtual void		identifyProlog(const std::string & id);
	virtual bool		popElement(void);
	virtual void		pushElement(void);
	virtual void		valueAttribute(const std::string & value);

	// Singleton Pattern - Resets the builder to its initial state
	virtual void reset(void);
};

#endif // BUILDER_H
