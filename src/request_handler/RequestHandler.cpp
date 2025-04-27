#include "RequestHandler.hpp"

RequestHandler::RequestHandler(int serverFd, Router& router, CgiHandler& cgiHandler): serverFd_(serverFd), router_(router), cgiHandler_(cgiHandler) {}

RequestHandler::~RequestHandler() {}

Response* RequestHandler::dispatch(const Request& request, int clientFd) {
	RouteResult routeResult = router_.routeRequest(request);
	
	if (routeResult.type == CGI_RESOURCE) {
		// cgi request
		cgiHandler_.processCgiRequest(request, clientFd, routeResult);
		return NULL;
	}
	if (routeResult.type == STATIC_RESOURCE) {
		// static resource request
		Response* response = StaticResourceHandler::execute(routeResult.filePath, routeResult.fileExtension, routeResult.statusCode);
		return response;
	}
	if (routeResult.type == REDIRECT_RESOURCE) {
		Response* response = StaticResourceHandler::execute(routeResult.filePath, routeResult.fileExtension, routeResult.statusCode);
		return response;
	}

	throw std::runtime_error("Error: Unknown route type");
}
