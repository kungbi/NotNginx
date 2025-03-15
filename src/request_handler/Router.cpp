#include "Router.hpp"

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
    std::cout << "\n=== 현재 저장된 라우팅 테이블 ===\n";
    for (std::vector<RoutePair>::const_iterator it = sortedRoutes_.begin(); it != sortedRoutes_.end(); ++it) {
        std::cout << "Path: " << it->first << " -> Root: " << it->second.getRoot() << std::endl;
    }
}

PathInfo Router::convertPath(const std::string& path, bool isCgi) {
    for (std::vector<RoutePair>::const_iterator it = sortedRoutes_.begin(); it != sortedRoutes_.end(); ++it) {
        if (path.find(it->first) == 0) { // 최장 접두사 매칭
            std::string scriptPath = it->second.getRoot() + (path.substr(it->first.length()));
            if (isCgi) {
                return PathInfo(scriptPath, it->second.getCgiInterpreter());
            }
            return PathInfo(scriptPath, "");
        }
    }
    return PathInfo("UNKNOWN", "");
}

