#include "CgiHandler.hpp"

CgiHandler::~CgiHandler() {}

CgiHandler::CgiHandler(Kqueue& kqueue) : kqueue_(kqueue), cgiExecuter_() {}

std::string CgiHandler::requestTypeToString(RequestType type) {
    switch (type) {
        case GET: return "GET";
        case POST: return "POST";
        case PUT: return "PUT";
        case PATCH: return "PATCH";
        case DELETE: return "DELETE";
        default:
            throw std::invalid_argument("Invalid RequestType encountered in requestTypeToString");
    }
}

void CgiHandler::processCgiRequest(const Request& request, int clientFd, PathInfo pathInfo) {
    std::string requestMethod = requestTypeToString(request.getRequestType());

    int outputFd = cgiExecuter_.run(pathInfo, request.getQuery(), requestMethod, request.getBody());

    if (outputFd < 0) {
        throw std::runtime_error("Failed to get valid output file descriptor from CgiExecuter");
    }
    kqueue_.addEvent(outputFd, KQUEUE_EVENT::CGI_RESPONSE, clientFd);
}
