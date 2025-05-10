#include "ConfigAdapter.hpp"


HTTPConfig *ConfigAdapter::convertToHTTPConfig(ConfigData& configData) {
	std::vector<ServerConfig*>* servers = new std::vector<ServerConfig*>(); // Initialize the vector of ServerConfig pointers
	IConfigContext* rootContext = configData.getRoot();

	if (!rootContext) {
		throw std::runtime_error("Config root context is null");
	}

	std::vector<IConfigContext*> serverContexts = rootContext->getChild();
	for (size_t i = 0; i < serverContexts.size(); ++i) {
		servers->push_back(new ServerConfig(convertToServerConfig(serverContexts[i]))); // Allocate dynamically
	}

	std::cout << "convertToHTTPConfig: Created " << servers->size() << " servers." << std::endl; // Access size of the vector correctly
	return new HTTPConfig(servers); // Pass the vector to HTTPConfig without address-of operator
}

ServerConfig ConfigAdapter::convertToServerConfig(IConfigContext* serverContext)
{
	std::string host;
	int port = 80;
	std::string serverName;
	size_t clientMaxBodySize = 1;
	std::map<int, std::string> errorPages;
	errorPages.clear(); // Ensure errorPages is cleared before use
	std::vector<LocationConfig> locations;
	bool isDefault = false;

	errorPages.clear();

	std::vector<IConfigDirective*> directives = serverContext->getDirectives();


	for (size_t i = 0; i < directives.size(); ++i)
	{
		int directiveType = directives[i]->getType();
		std::vector<std::string> values = directives[i]->getValues();
		
		switch (directiveType)
		{
			case HOST:
				if (!values.empty()) host = values[0];
				break;
			case PORT:
				if (!values.empty()) port = RequestParser::stoiSafe(values[0], 0);
				break;
			case SERVER_NAME:
				if (!values.empty()) serverName = values[0];
				break;
			case CLIENT_MAX_BODY_SIZE:
				if (!values.empty()) clientMaxBodySize = std::stoul(values[0]);
				break;
			case ERROR_PAGE:
				if (values.size() >= 2)
				{
					std::string errorPagePath = values.back(); // 마지막 값은 에러 페이지 경로
			
					for (size_t j = 0; j < values.size() - 1; ++j)  // 마지막 값 제외
					{
						int errorCode = RequestParser::stoiSafe(values[j], 0);
						errorPages[errorCode] = errorPagePath;  // 모든 상태 코드에 같은 페이지 경로 할당
					}
				}
				break;
			default:
				break;
		}
	}
	std::vector<IConfigContext*> locationContexts = serverContext->getChild();
	for (size_t i = 0; i < locationContexts.size(); ++i)
	{
		locations.push_back(convertToLocationConfig(locationContexts[i]));
	}

	return ServerConfig(host, port, serverName, clientMaxBodySize, errorPages, locations, isDefault);
}

LocationConfig ConfigAdapter::convertToLocationConfig(IConfigContext* locationContext)
{
	std::string pattern;
	std::string root;
	std::vector<std::string> allowMethods;
	bool autoindex = false;
	std::vector< std::pair<int, std::string> > redirect;
	std::vector<std::string> index;
	std::string cgiInterpreter;

	// pattern
	std::vector<std::string> options = locationContext->getOptions();
	if (!options.empty())
	{
		pattern = options[0];
	}

	std::vector<IConfigDirective*> directives = locationContext->getDirectives();
	for (size_t i = 0; i < directives.size(); ++i)
	{
		int directiveType = directives[i]->getType();
		std::vector<std::string> values = directives[i]->getValues();
		
		switch (directiveType)
		{
			case ROOT:
				if (!values.empty()) root = values[0];
				break;
			case ALLOW_METHODS:
				if (!values.empty()) allowMethods = values;
				break;
			case AUTOINDEX:
				if (!values.empty()) autoindex = (values[0] == "on");
				break;
			case INDEX:
				if (!values.empty()) index = values;
				break;
			case CGI_INTERPRETER:
				if (!values.empty()) cgiInterpreter = values[0];
				break;
			case RETURN:
				if (values.size() >= 2)
				{
					int statusCode = RequestParser::stoiSafe(values[0], 0);
					std::string redirectPath = values[1];
					redirect.push_back(make_pair(statusCode, redirectPath));
				}
				break;
			default:
				break;
		}
	}
	
	return LocationConfig(pattern, root, allowMethods, autoindex, redirect, index, cgiInterpreter, pattern);
}
