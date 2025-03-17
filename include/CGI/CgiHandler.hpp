#pragma once

#include "Request.hpp"
#include "Kqueue.hpp"
#include "CgiExecuter.hpp"

class CgiHandler {
private:
    Kqueue& kqueue_;
    CgiExecuter cgiExecuter_;

    std::string requestTypeToString(RequestType type);

public:
    CgiHandler(Kqueue& kqueue);
    ~CgiHandler();

    void processCgiRequest(const Request& request, int clientFd, std::string cgiScriptPath);
};
