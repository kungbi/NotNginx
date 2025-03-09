#pragma once

#include "Kqueue.hpp"

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sstream>

class CGIexecuter
{
	private:
		std::string& scriptPath_;
		std::string& queryString_;
		std::string& requestMethod_;
		std::string& requestBody_;

	public:
		CGIexecuter(
			const std::string& scriptPath,
			const std::string& queryString,
			const std::string& requestMethod,
			const std::string& requestBody
		);
		~CGIexecuter();
		
		void setEnvVariables();
		void executeCGIScript(Kqueue& kqueue);
};
