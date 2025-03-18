#include "Router.hpp"

Router::Router() {}

Router::Router(ServerConfig& serverConfig) {
    for (size_t i = 0; i < serverConfig.getLocations().size(); i++) {
        const LocationConfig& location = serverConfig.getLocations()[i];
        addRoute(location.getPattern(), location);
    }
    sortRoutes();
}

Router::~Router() {}

void Router::addRoute(const std::string& pattern, const LocationConfig& location) {
    routes_[pattern] = location;
}

bool Router::compareRouteLength(const RoutePair& a, const RoutePair& b) {
    return a.first.length() > b.first.length();
}

void Router::sortRoutes() {
    sortedRoutes_.assign(routes_.begin(), routes_.end());
    std::sort(sortedRoutes_.begin(), sortedRoutes_.end(), compareRouteLength);
}

void Router::printRoutes() {
    for (std::vector<RoutePair>::const_iterator it = sortedRoutes_.begin(); it != sortedRoutes_.end(); ++it) {
        std::cout << "Path: " << it->first << " -> Root: " << it->second.getRoot() << std::endl;
    }
}

PathInfo Router::convertPath(const std::string& path, const std::string& fileName) {
    bool testbool = fileName.rfind(".py") != std::string::npos;

    for (std::vector<RoutePair>::const_iterator it = sortedRoutes_.begin(); it != sortedRoutes_.end(); ++it) {
        // CGI
        if (testbool && it->first == ".py") {
            std::string scriptPath = it->second.getRoot() + path + "/" + fileName;
            return PathInfo(scriptPath, it->second.getCgiInterpreter());
        }
        // STATIC (최장접두사 매칭되어야 실행됨)
        if (path.find(it->first) == 0) {
            std::string scriptPath = it->second.getRoot() + (path.substr(it->first.length()));
            if (it->first == "/" && path.size() != 1) { // 접두사 / + 경로가 붙는경우
                scriptPath = it->second.getRoot() + "/" + (path.substr(it->first.length()));
            }
            if (!fileName.empty()) {
                scriptPath += "/" + fileName;
            }
            return PathInfo(scriptPath, "");
        }
    }
    throw std::runtime_error("Error: No route found for path: " + path);
}
