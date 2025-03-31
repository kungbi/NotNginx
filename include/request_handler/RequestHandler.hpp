#ifndef REQUEST_HANDLER_HPP
#define REQUEST_HANDLER_HPP

#include "Request.hpp"
#include "Router.hpp"
#include "CgiHandler.hpp"
#include "StaticResourceHandler.hpp"

class RequestHandler {
private:
    Router& router_;
    // CgiHandler& cgiHandler_;

public:
	RequestHandler(Router& router);
    ~RequestHandler();

    Response* dispatch(const Request& request);
};

#endif
