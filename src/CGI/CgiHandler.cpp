#include "CgiHandler.hpp"

CgiHandler::~CgiHandler() {}

CgiHandler::CgiHandler(Kqueue& kqueue, CgiExecuter& cgiExecuter) : kqueue_(kqueue), cgiExecuter_(cgiExecuter) {}

std::string requestTypeToString(RequestType type) {
    switch (type) {
        case GET: return "GET";
        case POST: return "POST";
        case PUT: return "PUT";
        case PATCH: return "PATCH";
        case DELETE: return "DELETE";
        default: return "UNKNOWN";
    }
}

void CgiHandler::handleRequest(const Request& request)
{
    std::string requestMethod = requestTypeToString(request.getRequestType());
    cgiExecuter_.executeCgiScript(request.getPath(), request.getQuery(), requestMethod, request.getBody());
    int outputFd = cgiExecuter_.getOutputFd();
    if (outputFd == -1) {
        throw std::runtime_error("Invalid output file descriptor from CgiExecuter");
    }
    kqueue_.addEvent(outputFd, KQUEUE_EVENT::RESPONSE, 0);
}