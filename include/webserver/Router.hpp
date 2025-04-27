#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "LocationConfig.hpp"
#include "ServerConfig.hpp"
#include "Request.hpp"
#include "HttpExceptions.hpp"

enum RouteType {
	STATIC_RESOURCE,
	CGI_RESOURCE,
	REDIRECT_RESOURCE,
};

struct RouteResult {
	RouteType type;
	std::string filePath;
	std::string cgiInterpreter;
	std::string fileExtension;

	RouteResult(RouteType routeType, const std::string& path, const std::string& interpreter = "", const std::string& extension = "")
		: type(routeType), filePath(path), cgiInterpreter(interpreter), fileExtension(extension) {}
};

class Router {
private:
	typedef std::pair<std::string, LocationConfig> LocationPair;

	std::vector<LocationPair> sortedLocations_;

	void sortLocations();
	static bool compareLength(const LocationPair& a, const LocationPair& b);
	bool matchPattern(const std::string& target, const std::string& pattern) const;
	std::string getRelativePath(const std::string& target, const std::string& pattern) const;
	std::string getFilePath(const LocationConfig& location, const std::string& relativePath, const std::string& fileName) const;
	RouteResult createResult(const LocationConfig& location, const std::string& filePath, const std::string& fileExtension) const;
	std::string getIndex(const LocationConfig& location) const;
	bool exists(const std::string& path) const;
	std::string getExtension(const std::string& fileName) const;
	bool isValidMethod(const RequestType& method, const LocationConfig& location) const;

public:
	Router(const ServerConfig& serverConfig);
	~Router();

	RouteResult routeRequest(const Request& request) const;
};

#endif
