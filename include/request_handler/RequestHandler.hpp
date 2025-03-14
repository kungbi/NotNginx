#pragma once

#include "Router.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "CgiHandler.hpp"
#include "CgiExecuter.hpp"
// #include "StaticResourceHandler.hpp"

class RequestHandler {
private:
    Router router_;
    CgiHandler cgiHandler_;
    // StaticResourceHandler staticHandler_;

public:
    RequestHandler(Kqueue& kqueue);
    ~RequestHandler();

    void handleRequest(const Request& request, int clientFd);
};
