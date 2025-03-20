#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include "LocationConfig.hpp"
#include "ServerConfig.hpp"

struct PathInfo {
    std::string scriptPath;
    std::string interpreter;

    PathInfo() {}
    PathInfo(const std::string& script, const std::string& interp)
        : scriptPath(script), interpreter(interp) {}
};

struct RouteData {
    std::string pattern;
    std::string root;
    std::string cgiInterpreter;

    RouteData() {}
    RouteData(const std::string& pattern, const std::string& root, const std::string& cgi)
        : pattern(pattern), root(root), cgiInterpreter(cgi) {}
};

class Router {
private:
    typedef std::pair<std::string, RouteData> RoutePair;
    std::map<std::string, RouteData> routes_;
    std::vector<RoutePair> sortedRoutes_;

    void sortRoutes();
    void addRoute(const LocationConfig& location);
    static bool compareRouteLength(const RoutePair& a, const RoutePair& b);
    static RouteData toRouteData(const LocationConfig& location);


public:
    Router(ServerConfig& serverConfig);
    ~Router();

    PathInfo convertPath(const std::string& path, const std::string& fileName);
    void printRoutes();
};

#endif // ROUTER_HPP
