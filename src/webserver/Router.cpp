#include "Router.hpp"

Router::Router(ServerConfig& serverConfig) {
    const std::vector<LocationConfig>& locs = serverConfig.getLocations();
    for (size_t i = 0; i < locs.size(); i++) {
        addRoute(locs[i]);
    }
    sortRoutes();
}

Router::~Router() {}

// LocationConfig를 RouteData로 변환
RouteData Router::toRouteData(const LocationConfig& location) {
    RouteData rd;
    rd.pattern = location.getPattern();
    rd.root = location.getRoot();
    rd.cgiInterpreter = location.getCgiInterpreter();
    return rd;
}

void Router::addRoute(const LocationConfig& location) {
    RouteData rd = toRouteData(location);
    routes_[rd.pattern] = rd;
}

bool Router::compareRouteLength(const RoutePair& a, const RoutePair& b) {
    return a.first.size() > b.first.size();
}

void Router::sortRoutes() {
    sortedRoutes_.clear();
    for (std::map<std::string, RouteData>::iterator it = routes_.begin();
         it != routes_.end(); ++it) {
        sortedRoutes_.push_back(*it);
    }
    std::sort(sortedRoutes_.begin(), sortedRoutes_.end(), compareRouteLength);
}

void Router::printRoutes() {
    std::cout << "\n=== 현재 저장된 라우팅 테이블 ===\n";
    for (size_t i = 0; i < sortedRoutes_.size(); i++) {
        const RouteData& rd = sortedRoutes_[i].second;
        std::cout << "Path: " << rd.pattern
                  << ", Root: " << rd.root
                  << ", CGI Interpreter: " << rd.cgiInterpreter << std::endl;
    }
}

RouteResult Router::convertPath(const std::string& path, const std::string& fileName) {
    bool isPy = (fileName.rfind(".py") != std::string::npos);

    // CGI 라우트 처리
    if (isPy && routes_.count(".py") > 0) {
        std::string combinedPath;
        if (path.empty()) {
            combinedPath = "/" + fileName;
        } else {
            combinedPath = path;
            if (!fileName.empty() && path[path.size() - 1] != '/') {
                combinedPath += "/";
            }
            combinedPath += fileName;
        }
        std::string scriptPath = routes_[".py"].root + combinedPath;
        return RouteResult(scriptPath, routes_[".py"].cgiInterpreter);
    }
    // 정적 라우트 처리
    for (size_t i = 0; i < sortedRoutes_.size(); i++) {
        const RouteData& rd = sortedRoutes_[i].second;
        if (path.find(rd.pattern) == 0) {
            std::string scriptPath = rd.root + path.substr(rd.pattern.size());

            if (rd.pattern == "/" && path.size() != 1) {
                scriptPath = rd.root + "/" + path.substr(1);
            }
            if (!fileName.empty()) {
                scriptPath += "/" + fileName;
            }
            return RouteResult(scriptPath, "");
        }
    }
    throw std::runtime_error("Error: No route found for path: " + path);
}
