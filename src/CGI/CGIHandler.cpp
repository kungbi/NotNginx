#include "CGIHandler.hpp"

CGIHandler::~CGIHandler() {}

CGIHandler::CGIHandler(const Request& request, Kqueue& kqueue) : request_(request), kqueue_(kqueue) {}

void CGIHandler::handleRequest()
{
    if (request_.getRequestType() != POST && request_.getRequestType() != GET)
	{
        throw std::runtime_error("Unsupported request type for CGI");
    }

    CGIexecuter cgiExec(
        request_.getPath(),
        request_.getQuery(),
        request_.getRequestType() == POST ? "POST" : "GET",
        request_.getBody()
    );

    cgiExec.executeCGIScript(kqueue_);
}


// bool CGIRequestHandler::isCGIRequest(const std::string& url)
// {
// 	return url.find(".php") != std::string::npos || url.find(".py") != std::string::npos;
// }

// std::string CGIRequestHandler::handleCGIRequest(const std::string& request, int clientFd)
// {
// 	CGIHandler cgiHandler;

// 	std::istringstream requestStream(request);
// 	std::string method, url, httpVersion;
// 	requestStream >> method >> url >> httpVersion; // HTTP 요청에서 메서드, URL, httpVersion을 추출.

// 	std::string scriptPath = "/var/www" + url;
// 	std::string queryString = "";

// 	// URL에 '?'가 포함되어 있으면 querystring 추출.
// 	// ex) "cgi-bin/script.py?name=John"
// 	size_t queryPos = url.find("?");
// 	if (queryPos != std::string::npos)
// 	{
// 		queryString = url.substr(queryPos + 1); // ex) "name=John"
// 		scriptPath = scriptPath.substr(0, queryPos); // ex)cgi-bin/script.py
// 	}

// 	// POST 요청 -> 본문을 저장
// 	// GET -> 그냥 빈 문자열
// 	std::string requestBody = "";
// 	if (method == "POST")
// 	{
// 		size_t contentLengthPos = request.find("Content-Length: "); // ex)"Content-Length: 11\r\n"
// 		if (contentLengthPos != std::string::npos)
// 		{
// 			size_t start = contentLengthPos + 16; // ex) "11"
// 			size_t end = request.find("\r\n, start");
// 			int contentLength = std::stoi(request.substr(start, end - start)); // 11을 정수로 변환

// 			char postBuffer[contentLength + 1];
// 			ssize_t receivedBytes = recv(clientFd, postBuffer, contentLength, 0);
// 			if (receivedBytes > 0)
// 			{
// 				postBuffer[receivedBytes] = '\0';
// 				requestBody = std::string(postBuffer);
// 			}
// 		}
// 	}
// 	return cgiHandler.executeCGIScript(scriptPath, queryString, method, requestBody);
// }