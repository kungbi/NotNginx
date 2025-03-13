#ifndef ROUTER_HPP
# define ROUTER_HPP

#include <string>
#include <map>

class Router {
private:
    std::map<std::string, std::string> routes;

public:
    Router();
    ~Router();

    void addRoute(const std::string& path, const std::string& handlerType);
    std::string getHandlerType(const std::string& path) const;
};


#endif