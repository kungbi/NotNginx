#pragma once

#include "Request.hpp"
#include "Kqueue.hpp"
#include "CGIExecuter.hpp"

class CGIHandler
{
	private:
		const Request& request_;
		Kqueue& kqueue_;
	
	public:
		CGIHandler(const Request& request, Kqueue& kqueue);
		~CGIHandler();
		
		void handleRequest();
};
