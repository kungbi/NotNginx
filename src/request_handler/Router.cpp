#include "../../include/request_handler/Router.hpp"

Router::Router(const std::string& configFile) {
    loadConfigRoute(configFile);
    sortRoutes();
}

Router::~Router() {}

void Router::addRoute(const std::string& path, const std::string& root) {
    routes_[path] = root;
}

std::string Router::getRootPath(const std::string& path) {
    for (std::vector<RoutePair>::const_iterator it = sortedRoutes_.begin(); it != sortedRoutes_.end(); ++it) {
        if (path.find(it->first) == 0) {
            return it->second;
        }
    }
    return "UNKNOWN";
}

void Router::loadConfigRoute(const std::string& configFile) {
    std::ifstream file(configFile);

    if (!file.is_open()) {
        std::cerr << "Error: Unable to open config file: " << configFile << std::endl;
        return;
    }

    std::string line, currentLocation, currentRoot;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key == "location") {
            if (!currentLocation.empty()) {
                addRoute(currentLocation, currentRoot);
            }
            iss >> currentLocation;
            currentRoot.clear();
        } else if (key == "root") {
            iss >> currentRoot;
            if (!currentRoot.empty() && currentRoot[currentRoot.size() - 1] == ';') {
                currentRoot = currentRoot.substr(0, currentRoot.size() - 1);
            }
        }
    }

    if (!currentLocation.empty()) {
        addRoute(currentLocation, currentRoot);
    }

    file.close();
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
        std::cout << "Path: " << it->first << " -> Root: " << it->second << std::endl;
    }
}
