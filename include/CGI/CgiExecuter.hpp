#pragma once

#include "CgiHandler.hpp"
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sstream>

class CgiExecuter
{
	private:
		int outputFd_;

	public:
		CgiExecuter();
		~CgiExecuter();
		
		void executeCgiScript(
			const std::string& scriptPath,
			const std::string& queryString,
			const std::string& requestMethod,
			const std::string& requestBody
		);
		void setEnvVariables(
			const std::string& scriptPath,
			const std::string& queryString,
			const std::string& requestMethod,
			const std::string& requestBody
		);
		int getOutputFd() const;
};
