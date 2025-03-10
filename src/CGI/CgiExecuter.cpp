#include "CgiExecuter.hpp"

CgiExecuter::CgiExecuter() : outputFd_(-1) {}

CgiExecuter::~CgiExecuter() {}

void CgiExecuter::setEnvVariables(
	const std::string& scriptPath,
	const std::string& queryString,
	const std::string& requestMethod,
	const std::string& requestBody
) {
	setenv("PATH_INFO", scriptPath.c_str(), 1);
	setenv("QUERY_STRING", queryString.c_str(), 1);
	setenv("REQUEST_METHOD", requestMethod.c_str(), 1);
	std::string contentLength = std::to_string(requestBody.size());
	setenv("CONTENT_LENGTH", contentLength.c_str(), 1);
}

void CgiExecuter::executeCgiScript(
	const std::string& scriptPath,
	const std::string& queryString,
	const std::string& requestMethod,
	const std::string& requestBody
) {
	int pipefd[2];

	if (pipe(pipefd) == -1) {
		throw std::runtime_error("Failed to create pipe");
	}
	outputFd_ = pipefd[0]; // 부모 프로세스에서 읽기 위한 FD 저장
	pid_t pid = fork();
	if (pid == -1) {
		throw std::runtime_error("Failed to fork Cgi process");
	}
	if (pid == 0) {  
		close(pipefd[0]); // 자식 프로세스는 읽기 엔드 닫음
		setEnvVariables(scriptPath, queryString, requestMethod, requestBody);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
        
		if (requestMethod == "POST") {
			int inputPipe[2];
			if (pipe(inputPipe) == -1) {
				exit(1);
			}
			if (write(inputPipe[1], requestBody.c_str(), requestBody.size()) == -1) {
				perror("write failed in CgiExecuter");
				close(inputPipe[1]);
				exit(1);
			}
			close(inputPipe[1]);
			dup2(inputPipe[0], STDIN_FILENO);
			close(inputPipe[0]);
		}
		execlp("python3", "python3", scriptPath.c_str(), NULL);
		perror("execlp");
		exit(1);
	} else {
		close(pipefd[1]);
	}
}

int CgiExecuter::getOutputFd() const {
	return outputFd_;
}
#include "CgiExecuter.hpp"

CgiExecuter::CgiExecuter() : outputFd_(-1) {}

CgiExecuter::~CgiExecuter() {}

void CgiExecuter::setEnvVariables(
	const std::string& scriptPath,
	const std::string& queryString,
	const std::string& requestMethod,
	const std::string& requestBody
) {
	setenv("PATH_INFO", scriptPath.c_str(), 1);
	setenv("QUERY_STRING", queryString.c_str(), 1);
	setenv("REQUEST_METHOD", requestMethod.c_str(), 1);
	std::string contentLength = std::to_string(requestBody.size());
	setenv("CONTENT_LENGTH", contentLength.c_str(), 1);
}

void CgiExecuter::executeCgiScript(
	const std::string& scriptPath,
	const std::string& queryString,
	const std::string& requestMethod,
	const std::string& requestBody
) {
	int pipefd[2];

	if (pipe(pipefd) == -1) {
		throw std::runtime_error("Failed to create pipe");
	}
	outputFd_ = pipefd[0]; // 부모 프로세스에서 읽기 위한 FD 저장
	pid_t pid = fork();
	if (pid == -1) {
		throw std::runtime_error("Failed to fork Cgi process");
	}
	if (pid == 0) {  
		close(pipefd[0]); // 자식 프로세스는 읽기 엔드 닫음
		setEnvVariables(scriptPath, queryString, requestMethod, requestBody);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
        
		if (requestMethod == "POST") {
			int inputPipe[2];
			if (pipe(inputPipe) == -1) {
				exit(1);
			}
			if (write(inputPipe[1], requestBody.c_str(), requestBody.size()) == -1) {
				perror("write failed in CgiExecuter");
				close(inputPipe[1]);
				exit(1);
			}
			close(inputPipe[1]);
			dup2(inputPipe[0], STDIN_FILENO);
			close(inputPipe[0]);
		}
		execlp("python3", "python3", scriptPath.c_str(), NULL);
		perror("execlp");
		exit(1);
	} else {
		close(pipefd[1]);
	}
}

int CgiExecuter::getOutputFd() const {
	return outputFd_;
}
