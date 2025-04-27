#include "CgiExecuter.hpp"

CgiExecuter::CgiExecuter() {
	setupSigchldHandler();
}

CgiExecuter::~CgiExecuter() {}

int CgiExecuter::run(RouteResult routeResult, const std::string& queryString,
                     const std::string& requestMethod, const std::string& requestBody) {
	CgiRequestData requestData(routeResult, queryString, requestMethod, requestBody);
	
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		throw std::runtime_error("Failed to create pipe");
	}

	pid_t pid = fork();
	if (pid == -1) {
		throw std::runtime_error("Failed to fork CGI process");
	}

	if (pid == 0) {
		// Child process
		close(pipefd[0]); // 부모가 읽는 쪽 닫기
		executeChild(pipefd[1], requestData);
		// 절대 여기까지 오면 안 됨
		exit(1);
	}

	// Parent process
	close(pipefd[1]); // 자식이 쓰는 쪽 닫기
	return pipefd[0]; // 읽기용 fd 리턴
}

void CgiExecuter::executeChild(int writeFd, const CgiRequestData& requestData) {
	// stdout -> writeFd 로 리다이렉션
	if (dup2(writeFd, STDOUT_FILENO) == -1) {
		perror("dup2 to STDOUT failed");
		exit(1);
	}
	close(writeFd);

	// body를 stdin으로 넘김
	if (isMethodWithBody(requestData.requestMethod)) {
		setupBodyPipe(requestData.requestBody);
	}

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
		perror("pipe");
		exit(1);
	}

	if (write(inputPipe[1], requestBody.c_str(), requestBody.size()) == -1) {
		perror("write to inputPipe failed");
		exit(1);
	}
	close(inputPipe[1]);

	if (dup2(inputPipe[0], STDIN_FILENO) == -1) {
		perror("dup2 to STDIN failed");
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
		perror("sigaction failed");
		exit(1);
	}
}
