#include "RequestHandler.hpp"

RequestHandler::RequestHandler(Router& router): router_(router) {}

RequestHandler::~RequestHandler() {}

Response* RequestHandler::dispatch(const Request& request) {
	RouteResult routeResult = router_.routeRequest(request);
	std::cout << "Route result type: " << routeResult.type << std::endl;
	std::cout << "Route result file path: " << routeResult.filePath << std::endl;
	std::cout << "Route result CGI interpreter: " << routeResult.cgiInterpreter << std::endl;
	std::cout << "Route result file extension: " << routeResult.fileExtension << std::endl;


	if (routeResult.type == CGI_RESOURCE) {
		// cgi request
		// cgiHandler_.processCgiRequest(request, clientFd, routeResult);
		return NULL;
	}
	if (routeResult.type == STATIC_RESOURCE) {
		// static resource request
		Response* response = StaticResourceHandler::execute(routeResult.filePath, routeResult.fileExtension);
		return response;
	}

	throw std::runtime_error("Error: Unknown route type");
}
