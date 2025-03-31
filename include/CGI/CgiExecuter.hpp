#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sstream>
#include <signal.h>
#include "Router.hpp"

struct PathInfo {
    std::string interpreter;   // CGI 스크립트를 실행할 인터프리터 경로 (예: /usr/bin/python)
    std::string scriptPath;    // 실행할 CGI 스크립트의 경로 (예: /var/www/cgi-bin/script.py)
    std::string requestPath;   // 클라이언트가 요청한 경로 (예: /cgi-bin/script.py)
    std::string additionalPath; // 추가적인 경로 정보 (예: /extra/path)

    // 기본 생성자
    PathInfo() {}

    // 매개변수를 받는 생성자
    PathInfo(const std::string& interpreter, const std::string& scriptPath, const std::string& requestPath, const std::string& additionalPath)
        : interpreter(interpreter), scriptPath(scriptPath), requestPath(requestPath), additionalPath(additionalPath) {}
};

struct CgiRequestData {
	RouteResult routeResult;
	std::string queryString;
	std::string requestMethod;
	std::string requestBody;
	
	CgiRequestData(RouteResult routeResult, const std::string& query, const std::string& method, const std::string& body)
		: routeResult(routeResult) ,queryString(query), requestMethod(method), requestBody(body) {}
};

class CgiExecuter {
private:
	void setEnvVariables(const CgiRequestData& requestData); // 환경 변수 설정 (요청 데이터 구조체 전달)
	void setupBodyPipe(const std::string& requestBody); // 본문 데이터를 처리하는 파이프 설정
	void executeChild(int pipefd[2], const CgiRequestData& requestData); // 자식 프로세스 실행 (요청 데이터 전달)
	void checkChildStatus(pid_t pid); // 자식 프로세스 종료 상태 확인

	static void handleSigchld(int signo); // `SIGCHLD` 핸들러
	void setupSigchldHandler(); // `SIGCHLD` 핸들러 설정

public:
	CgiExecuter();
	~CgiExecuter();

	int run(
		RouteResult routeResult,
		const std::string& queryString,
		const std::string& requestMethod,
		const std::string& requestBody
	);
};
