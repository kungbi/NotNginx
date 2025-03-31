#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "LocationConfig.hpp"
#include "ServerConfig.hpp"
#include "Request.hpp"

// Enum to represent the type of route
enum RouteType {
    STATIC_RESOURCE,
    CGI_RESOURCE
};

// Structure to store the result of routing
struct RouteResult {
    RouteType type;             // Type of the route (STATIC_RESOURCE or CGI_RESOURCE)
    std::string filePath;       // Path to the file (CGI script or static resource)
    std::string cgiInterpreter; // Path to the CGI interpreter (if applicable)
    std::string fileExtension;  // File extension (if applicable)

    RouteResult(RouteType routeType, const std::string& path, const std::string& interpreter = "", const std::string& extension = "")
        : type(routeType), filePath(path), cgiInterpreter(interpreter), fileExtension(extension) {}
};

class Router {
private:
    typedef std::pair<std::string, LocationConfig> LocationPair;

    // Member variables
    std::vector<LocationPair> sortedLocations_;

    // Helper functions
    void sortLocations();
    static bool compareLocationLength(const LocationPair& a, const LocationPair& b);
    bool isPatternMatch(const std::string& target, const std::string& pattern) const;
    std::string calculateRelativePath(const std::string& target, const std::string& pattern) const;
    std::string buildFilePath(const LocationConfig& location, const std::string& relativePath, const std::string& fileName) const;
    RouteResult createRouteResult(const LocationConfig& location, const std::string& filePath, const std::string& fileExtension) const; // Updated signature
    std::string getIndexFile(const LocationConfig& location) const;
    bool fileExists(const std::string& path) const;
    std::string extractFileExtension(const std::string& fileName) const;

public:
    // Constructor
    Router(const ServerConfig& serverConfig);
    ~Router();

    // Public method to handle routing
    RouteResult routeRequest(const Request& request) const;
};

#endif // ROUTER_HPP
