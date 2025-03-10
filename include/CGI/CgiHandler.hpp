#pragma once

#include "Request.hpp"
#include "Kqueue.hpp"
#include "CgiExecuter.hpp"

class CgiHandler
{
	private:
		Kqueue& kqueue_;
		CgiExecuter& cgiExecuter_;

	public:
		CgiHandler(Kqueue& kqueue, CgiExecuter& cgiExecuter);
		~CgiHandler();
		
		void handleRequest(const Request& request);
};

std::string requestTypeToString(RequestType type);