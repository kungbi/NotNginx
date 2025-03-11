#include "CgiExecuter.hpp"

CgiExecuter::CgiExecuter() {}

CgiExecuter::~CgiExecuter() {}

int CgiExecuter::executeCgiScript(
	const std::string& scriptPath,
	const std::string& queryString,
	const std::string& requestMethod,
	const std::string& requestBody
) {
	CgiRequestData requestData(scriptPath, queryString, requestMethod, requestBody); // 각 요청마다 새로운 요청 데이터를 생성하여 사용

	int pipefd[2];
	if (pipe(pipefd) == -1)
		throw std::runtime_error("Failed to create pipe");

	pid_t pid = fork();
	if (pid == -1)
		throw std::runtime_error("Failed to fork Cgi process");

	if (pid == 0) {
		executeChild(pipefd, requestData);
		exit(1);
	}
	
	close(pipefd[1]);
	checkChildStatus(pid);
	return pipefd[0];
}

void CgiExecuter::executeChild(int pipefd[2], const CgiRequestData& requestData) {
	close(pipefd[0]);
	setEnvVariables(requestData);
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[1]);
	
	if (requestData.requestMethod == "POST" || requestData.requestMethod == "PUT" || requestData.requestMethod == "PATCH") {
		setupBodyPipe(requestData.requestBody);
	}

	execlp("python3", "python3", requestData.scriptPath.c_str(), NULL);
	perror("execlp");
	exit(1);
}

void CgiExecuter::setEnvVariables(const CgiRequestData& requestData) {
	setenv("PATH_INFO", requestData.scriptPath.c_str(), 1);
	setenv("QUERY_STRING", requestData.queryString.c_str(), 1);
	setenv("REQUEST_METHOD", requestData.requestMethod.c_str(), 1);
	
	if (requestData.requestMethod == "POST" || requestData.requestMethod == "PUT" || requestData.requestMethod == "PATCH") {
		std::string contentLength = std::to_string(requestData.requestBody.size());
		setenv("CONTENT_LENGTH", contentLength.c_str(), 1);
	}
}

void CgiExecuter::setupBodyPipe(const std::string& requestBody) {
	int inputPipe[2];
	if (pipe(inputPipe) == -1) {
		throw std::runtime_error("Failed to create pipe");
	}
	if (write(inputPipe[1], requestBody.c_str(), requestBody.size()) == -1) {
		perror("write failed in CgiExecuter");
		close(inputPipe[1]);
		close(inputPipe[0]);
		throw std::runtime_error("Failed to write request body to CGI");
	}
	close(inputPipe[1]);
	if (dup2(inputPipe[0], STDIN_FILENO) == -1) {
		perror("dup2 failed in setupBodyPipe");
		close(inputPipe[0]); // dup2 실패 시 FD 닫기
		throw std::runtime_error("Failed to redirect CGI input");
	}
	close(inputPipe[0]);
	
}

void CgiExecuter::checkChildStatus(pid_t pid) {
    int status;
    int retries = 100; // 최대 100회 (1000ms = 1초)

    while (retries-- > 0) {
        pid_t result = waitpid(pid, &status, WNOHANG);
        
        if (result > 0) { // 자식 프로세스가 종료됨
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                throw std::runtime_error("CGI execution failed.");
            }
            return;
        } else if (result == 0) { // 자식 프로세스가 아직 실행 중
            usleep(10000);  // 10ms 대기 후 다시 확인
        } else {
            throw std::runtime_error("Error waiting for CGI process.");
        }
    }

	if (kill(pid, SIGKILL) == -1) {
		perror("kill failed in checkChildStatus");
		throw std::runtime_error("Failed to terminate CGI process.");
	}
	
    throw std::runtime_error("CGI execution timed out and was forcibly terminated.");
}
