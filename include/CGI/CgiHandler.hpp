#pragma once

#include "Request.hpp"
#include "Kqueue.hpp"
#include "CgiExecuter.hpp"

class CgiHandler {
private:
	Kqueue& kqueue_;
	CgiExecuter& cgiExecuter_;
	
	std::string requestTypeToString(RequestType type);

public:
	CgiHandler(Kqueue& kqueue, CgiExecuter& cgiExecuter);
	~CgiHandler();
	
	void handleRequest(const Request& request, int clientFd);
};
