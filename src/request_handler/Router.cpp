#include "Router.hpp"

Router::Router() {}

Router::~Router() {}

void Router::addRoute(const std::string& path, const std::string& handlerType) {
    routes[path] = handlerType;
}

std::string Router::getHandlerType(const std::string& path) const {
    std::map<std::string, std::string>::const_iterator it = routes.begin();
    for (it; it != routes.end(); it++) {
        if (path.find(it->first) == 0) {
            return it->second;
        }
    }
    return "Unknown"; // 둘다 아닌경우
}