cmake_minimum_required (VERSION 3.23)
project (Patterns)

add_executable(Patterns Test.C)
set_property(TARGET Patterns PROPERTY CXX_STANDARD 20)
target_sources(Patterns PUBLIC
	Attr.C
	Attr.H
	Builder.C
	Builder.H
	Director.C
	Director.H
	Document.C
	Document.H
	Element.C
	Element.H
	NamedNodeMap.C
	NamedNodeMap.H
	Node.C
	Node.H
	NodeList.H
	Test.C
	Text.C
	Text.H
	XMLSerializer.C
	XMLSerializer.H
	XMLTokenizer.C
	XMLTokenizer.H
	XMLValidator.C
	XMLValidator.H
	ValidChildren.C
	ValidChildren.H
	WhitespaceStrategy.H
	)
