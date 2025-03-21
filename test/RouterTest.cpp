#include "../include/request_handler/Router.hpp"
#include <iostream>

int main() {
    Router router("../default2.conf");

    std::cout << "\n=== 라우팅 테이블 확인 ===\n";

    std::string testPaths[] = {"/404.html", "/asdfa/api", "/", "/api/fdjla/fjdklksa", "dfjkladf/fdafd/fd"};

    for (size_t i = 0; i < sizeof(testPaths) / sizeof(std::string); i++) {
        std::string root = router.getRootPath(testPaths[i]);
        std::cout << "Path: " << testPaths[i] << " -> Root: " << root << std::endl;
    }

    return 0;
}
