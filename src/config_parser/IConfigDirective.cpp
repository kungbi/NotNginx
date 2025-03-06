#include "IConfigDirective.hpp"

IConfigDirective::IConfigDirective() {}

IConfigDirective::IConfigDirective(IConfigContext *parent, int directiveType)
	: directiveType_(directiveType)
	, parent_(parent)
{}

IConfigDirective::~IConfigDirective() {}




// Getters
IConfigContext* IConfigDirective::getParent() const
{
	return parent_;
}

int IConfigDirective::getType() const
{
	return directiveType_;
}

std::vector<std::string> IConfigDirective::getValues() const
{
	return values_;
}





void IConfigDirective::addValue(std::string value)
{
	values_.push_back(value);
}

int isDirective(std::string token)
{
	std::vector<std::string> DirectiveStrings;
	DirectiveStrings.push_back("host");
	DirectiveStrings.push_back("port");
	DirectiveStrings.push_back("worker_processes");
	DirectiveStrings.push_back("error_page");
	DirectiveStrings.push_back("listen");
	DirectiveStrings.push_back("server_name");
	DirectiveStrings.push_back("root");
	DirectiveStrings.push_back("index");
	DirectiveStrings.push_back("allow_methods");
	DirectiveStrings.push_back("access_log");
	DirectiveStrings.push_back("autoindex");
	DirectiveStrings.push_back("fastcgi_pass");
	DirectiveStrings.push_back("fastcgi_index");
	DirectiveStrings.push_back("fastcgi_param");
	DirectiveStrings.push_back("client_max_body_size");
	DirectiveStrings.push_back("return");
	DirectiveStrings.push_back("include");
	DirectiveStrings.push_back("cgi_interpreter");

	for (size_t i = 0; i < DirectiveStrings.size(); ++i)
	{
		if (DirectiveStrings[i] == token)
			return i;
	}
	
	return -1;
}