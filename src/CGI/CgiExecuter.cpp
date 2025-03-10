#include "CGIExecuter.hpp"

CGIexecuter::~CGIexecuter() {}

CGIexecuter::CGIexecuter(
    const std::string& scriptPath,
    const std::string& queryString,
    const std::string& requestMethod,
    const std::string& requestBody
) : scriptPath_(scriptPath), queryString_(queryString), requestMethod_(requestMethod), requestBody_(requestBody) {}

void CGIexecuter::setEnvVariables()
{
	setenv("PATH_INFO", scriptPath_.c_str(), 1);
    setenv("QUERY_STRING", queryString_.c_str(), 1);
    setenv("REQUEST_METHOD", requestMethod_.c_str(), 1);
    std::string contentLength = std::to_string(requestBody_.size());
	setenv("CONTENT_LENGTH", std::to_string(requestBody_.size()).c_str(), 1);
}

void CGIexecuter::executeCGIScript(Kqueue& kqueue)
{
	int pipefd[2]; // 부모-자식 프로세스 간 통신용 파이프

    if (pipe(pipefd) == -1) 
	{
		throw std::runtime_error("Failed to create pipe");
	};

    pid_t pid = fork();
    if (pid == -1)
	{
		throw std::runtime_error("Failed to fork CGI process");
	}

    if (pid == 0) // 자식 프로세스 (CGI 실행)
	{  
        close(pipefd[0]);
        setEnvVariables();  // 환경 변수 설정
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (requestMethod_ == "POST")
		{
            int inputPipe[2];
            if (pipe(inputPipe) == -1) exit(1);
            
            if (write(inputPipe[1], requestBody_.c_str(), requestBody_.size()) == -1)
            {
                perror("write failed in CGIExecuter");
                close(inputPipe[1]);
                exit(1);
            }
            close(inputPipe[1]);
            dup2(inputPipe[0], STDIN_FILENO);
            close(inputPipe[0]);
        }
        execlp("python3", "python3", scriptPath_.c_str(), NULL);
		perror("execlp");
        exit(1);
    }
	else
	{  // 부모 프로세스 (웹 서버, Kqueue에 등록)
        close(pipefd[1]);  // 쓰기 엔드 닫기
        // 변경: CGI 결과가 파이프의 읽기 엔드 (pipefd[0]) -> Kqueue에 등록
        kqueue.addEvent(pipefd[0], KQUEUE_EVENT::RESPONSE, pid);
    }
}
