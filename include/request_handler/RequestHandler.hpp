#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include "Request.hpp"
#include "Router.hpp"
#include "CgiHandler.hpp"
#include "StaticResourceHandler.hpp"

class RequestHandler {
private:
	int serverFd_;
    Router& router_;
    CgiHandler& cgiHandler_;

public:
	RequestHandler(int serverFd, Router& router, CgiHandler& cgiHandler);
    ~RequestHandler();

    Response* dispatch(const Request& request, int clientFd);
};

#endif
