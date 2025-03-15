#ifndef ROUTER_HPP
# define ROUTER_HPP

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

typedef std::pair<std::string, std::string> RoutePair;

class Router {
private:
    std::map<std::string, std::string> routes_;
    std::vector<RoutePair> sortedRoutes_; // 최종 정렬된 라우트 테이블(최장접두사 고려)

    void loadConfigRoute(const std::string& configFile);
    void sortRoutes();
    static bool compareRouteLength(const RoutePair& a, const RoutePair& b);
    void printRoutes(); // 확인용 출력문

public:
    Router(const std::string& configFile);
    ~Router();

    void addRoute(const std::string& path, const std::string& root);
    std::string getRootPath(const std::string& path);
};

#endif
