#include "CgiExecuter.hpp"

CgiExecuter::CgiExecuter() {
    setupSigchldHandler();
}

CgiExecuter::~CgiExecuter() {}

int CgiExecuter::run(PathInfo pathInfo, const std::string& queryString, const std::string& requestMethod, const std::string& requestBody) {
    CgiRequestData requestData(pathInfo, queryString, requestMethod, requestBody);
    
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
    
    execlp(requestData.pathInfo.interpreter.c_str(), requestData.pathInfo.interpreter.c_str(), requestData.pathInfo.scriptPath.c_str(), NULL);
    perror("execlp");
    exit(1);
}

void CgiExecuter::setEnvVariables(const CgiRequestData& requestData) {
    setenv("PATH_INFO", requestData.pathInfo.scriptPath.c_str(), 1);
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

// `SIGCHLD` 핸들러: 종료된 자식 프로세스를 자동으로 수거
void CgiExecuter::handleSigchld(int signo) {
    (void)signo;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

// 웹서버 시작 시 `SIGCHLD` 핸들러 설정
void CgiExecuter::setupSigchldHandler() {
    struct sigaction sa;
    sa.sa_handler = CgiExecuter::handleSigchld;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGCHLD, &sa, NULL);
}