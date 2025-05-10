#include "CgiExecuter.hpp"

CgiExecuter::CgiExecuter() {
	setupSigchldHandler();
}

CgiExecuter::~CgiExecuter() {}

int CgiExecuter::run(RouteResult routeResult, const std::string& queryString,
                     const std::string& requestMethod, const std::string& requestBody, const std::string& contentType) {
	CgiRequestData requestData(routeResult, queryString, requestMethod, requestBody, contentType);
	
	int stdinPipe[2];
	int stdoutPipe[2];
	if (pipe(stdinPipe) == -1 || pipe(stdoutPipe) == -1) {
		throw std::runtime_error("Failed to create pipes");
	}

	pid_t pid = fork();
	if (pid == -1) {
		throw std::runtime_error("Failed to fork");
	}

	if (pid == 0) {
		// child
		close(stdinPipe[1]); // 부모가 쓰는 쪽 닫기
		dup2(stdinPipe[0], STDIN_FILENO);
		close(stdinPipe[0]);

		close(stdoutPipe[0]); // 부모가 읽는 쪽 닫기
		dup2(stdoutPipe[1], STDOUT_FILENO);
		close(stdoutPipe[1]);

		executeChild(requestData);
		exit(1);
	} 
	// parent
	close(stdinPipe[0]);  // 자식이 읽는 쪽 닫기
	close(stdoutPipe[1]); // 자식이 쓰는 쪽 닫기

	write(stdinPipe[1], requestBody.data(), requestBody.size());
	close(stdinPipe[1]); // body 다 쓰고 닫기

	return stdoutPipe[0]; // CGI 응답 읽기용 fd 리턴
}

void CgiExecuter::executeChild(const CgiRequestData& requestData) {
    // 환경변수 세팅
    setEnvVariables(requestData);

    // CGI 실행
    execlp(requestData.routeResult.cgiInterpreter.c_str(),
           requestData.routeResult.cgiInterpreter.c_str(),
           requestData.routeResult.filePath.c_str(),
           NULL);

    throw InternalServerError("Failed to execute CGI script");
}

void CgiExecuter::setupBodyPipe(const std::string& requestBody) {
	int inputPipe[2];
	if (pipe(inputPipe) == -1) {
		exit(1);
	}

	if (write(inputPipe[1], requestBody.c_str(), requestBody.size()) == -1) {
		exit(1);
	}
	close(inputPipe[1]);

	if (dup2(inputPipe[0], STDIN_FILENO) == -1) {
		exit(1);
	}
	close(inputPipe[0]);
}

void CgiExecuter::setEnvVariables(const CgiRequestData& requestData) {
	setenv("PATH_INFO", requestData.routeResult.filePath.c_str(), 1);
	setenv("QUERY_STRING", requestData.queryString.c_str(), 1);
	setenv("REQUEST_METHOD", requestData.requestMethod.c_str(), 1);

	if (isMethodWithBody(requestData.requestMethod)) {
		std::string contentLength = std::to_string(requestData.requestBody.size());
		setenv("CONTENT_LENGTH", contentLength.c_str(), 1);
	}
	if (!requestData.contentType.empty()) {
		setenv("CONTENT_TYPE", requestData.contentType.c_str(), 1);
	}
}

bool CgiExecuter::isMethodWithBody(const std::string& method) const {
	return method == "POST" || method == "PUT" || method == "PATCH";
}

void CgiExecuter::handleSigchld(int signo) {
	(void)signo;
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

void CgiExecuter::setupSigchldHandler() {
	struct sigaction sa;
	sa.sa_handler = CgiExecuter::handleSigchld;
	sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		exit(1);
	}
}
