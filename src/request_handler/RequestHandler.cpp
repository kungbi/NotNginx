#include "RequestHandler.hpp"

RequestHandler::RequestHandler(Kqueue& kqueue) 
    : cgiHandler_(kqueue) {
    router_.addRoute("/cgi-bin/", "CGI");
    router_.addRoute("/static/", "Static");
}

RequestHandler::~RequestHandler() {}

void RequestHandler::handleRequest(const Request& request, int clientFd) {
    std::string handlerType = router_.getHandlerType(request.getPath());

    if (handlerType == "CGI") {
        cgiHandler_.handleCgiRequest(request, clientFd);
    } else if (handlerType == "Static") {
        // Todo: Implement static resource handler
        // Ex) staticHandler_.serveStaticResource(request.getPath(), clientFd);
    } else {
        // Todo: Implement 404 handler
    }
}
