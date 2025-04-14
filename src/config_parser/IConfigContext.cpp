#include "IConfigContext.hpp"

IConfigContext::IConfigContext(IConfigContext *parent, int contextType)
	: parent_(parent)
	, contextType_(contextType)
{
	if (parent)
		parent->addChild(this);;
}

IConfigContext::~IConfigContext() {
	for (size_t i = 0; i < directives_.size(); ++i)
		delete directives_[i];
	for (size_t i = 0; i < child_.size(); ++i)
		delete child_[i];
}



// Getters
IConfigContext* IConfigContext::getParent() const
{
	return (parent_);
}

int IConfigContext::getType() const
{
	return (contextType_);
}

std::vector<IConfigContext *> IConfigContext::getChild() const
{
	return (child_);
}	

std::vector<IConfigDirective *> IConfigContext::getDirectives() const
{
	return (directives_);
}	

std::vector<std::string> IConfigContext::getOptions() const
{
	return (options_);
}



void IConfigContext::addDirectives(IConfigDirective *directive)
{
	directives_.push_back(directive);
}

void IConfigContext::addChild(IConfigContext *child)
{
	child_.push_back(child);
}

void IConfigContext::addOptions(std::string token)
{
	options_.push_back(token);
}

bool IConfigContext::isValid() const
{
	return (true);
}

void deleteTree(IConfigContext *root)
{
	if (!root)
		return ;
	
	for (size_t i = 0; i < root->getChild().size(); ++i)
		deleteTree(root->getChild()[i]);
	for (size_t i = 0; i < root->getDirectives().size(); ++i)
		delete root->getDirectives()[i];
	delete root;
}

int isContext(std::string token)
{
	std::vector<std::string> ContextStrings;
	ContextStrings.push_back("main");
	ContextStrings.push_back("http");
	ContextStrings.push_back("server");
	ContextStrings.push_back("events");
	ContextStrings.push_back("location");
	

	for (size_t i = 0; i < ContextStrings.size(); ++i)
	{
		if (token == ContextStrings[i]) return (i);
	}
	return (-1);
}
