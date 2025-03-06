#pragma once

#include "IConfigContext.hpp"

class IConfigContext;

enum DirectiveType
{
	HOST,
	PORT,
	WORKER_PROCESSES,
	ERROR_PAGE,
	LISTEN,
	SERVER_NAME,
	ROOT,
	INDEX,
	ALLOW_METHODS,
	ACCESS_LOG,
	AUTOINDEX,
	FASTCGI_PASS,
	FASTCGI_INDEX,
	FASTCGI_PARAM,
	CLIENT_MAX_BODY_SIZE,
	RETURN,
	INCLUDE,
	CGI_INTERPRETER,
};

class IConfigDirective
{
	private:
		int directiveType_;
		IConfigContext *parent_;
		std::vector<std::string> values_;
		
		IConfigDirective();

	public:
		IConfigDirective(IConfigContext *parent, int directiveType);
		~IConfigDirective();

		IConfigContext* getParent() const;
		int getType() const;
		

		void addValue(std::string value);
		std::vector<std::string> getValues() const;
};

int isDirective(std::string token);