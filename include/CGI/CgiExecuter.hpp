#pragma once

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sstream>
#include <signal.h>

struct CgiRequestData {
	std::string scriptPath;
	std::string queryString;
	std::string requestMethod;
	std::string requestBody;

	CgiRequestData() {}

	// 구조체에 생성자 추가 (초기화 편의성 제공)
	CgiRequestData(const std::string& script, const std::string& query, const std::string& method, const std::string& body)
		: scriptPath(script), queryString(query), requestMethod(method), requestBody(body) {}
};

class CgiExecuter
{
	private:
		void setEnvVariables(const CgiRequestData& requestData); // 환경 변수 설정 (요청 데이터 구조체 전달)
		void setupBodyPipe(const std::string& requestBody); // 본문 데이터를 처리하는 파이프 설정
		void executeChild(int pipefd[2], const CgiRequestData& requestData); // 자식 프로세스 실행 (요청 데이터 전달)
		void checkChildStatus(pid_t pid); // 자식 프로세스 종료 상태 확인

	public:
		CgiExecuter();
		~CgiExecuter();

		int executeCgiScript(
			const std::string& scriptPath,
			const std::string& queryString,
			const std::string& requestMethod,
			const std::string& requestBody
		);
};
