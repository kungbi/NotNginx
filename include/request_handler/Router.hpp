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

    PathInfo(const std::string& script, const std::string& interp) : scriptPath(script), interpreter(interp) {}
};


class Router {
public:
    Router(ServerConfig& serverConfig);
    ~Router();

    PathInfo convertPath(const std::string& path, bool isCgi);
    void printRoutes();
    
private:
    typedef std::pair<std::string, LocationConfig> RoutePair;
    std::map<std::string, LocationConfig> routes_;
    std::vector<RoutePair> sortedRoutes_;
    
    void sortRoutes();
    void addRoute(const std::string& pattern, const LocationConfig& location);
    static bool compareRouteLength(const RoutePair& a, const RoutePair& b);
};

#endif // ROUTER_HPP
