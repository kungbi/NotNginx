#include "RequestHandler.hpp"

RequestHandler::RequestHandler(Router& router, CgiHandler& cgiHandler)
    : router_(router), cgiHandler_(cgiHandler) {}

RequestHandler::~RequestHandler() {}

Response* RequestHandler::dispatch(const Request& request, int clientFd) {
    const std::string& extension = request.getExtension();
    const std::string& path = request.getPath();
    const std::string& fileName = request.getFilename();

	RouteResult routeResult = router_.convertPath(path, fileName);

	if (!routeResult.interpreter.empty()) {
		// cgi request
		cgiHandler_.processCgiRequest(request, clientFd, routeResult);
		return NULL;
	} else {
		// static resource request
		Response* response = StaticResourceHandler::execute(routeResult.scriptPath, extension);
		return response;
	}	
}
