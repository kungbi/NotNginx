#include "RequestParser.hpp"
#include <string>
#include "Request.hpp"
#include <iostream>

void testRequest(const std::string& requestStr) {
	Request request = RequestParser::parseRequestHeader(requestStr);
	std::cout << "Request Type: " << request.getRequestType() << std::endl;
	std::cout << "Protocol Version: " << request.getProtocolVersion() << std::endl;
	std::cout << "Host: " << request.getHost() << std::endl;
	std::cout << "Target: " << request.getTarget() << std::endl;
	std::cout << "Port: " << request.getPort() << std::endl;
	std::cout << "Connection: " << request.getConnection() << std::endl;
	std::cout << "Content Length: " << request.getContentLength() << std::endl;
	std::cout << "Accept: " << request.getAccept() << std::endl;
	std::cout << "Content-Type: " << request.getContentType() << std::endl;
	std::cout << "Query: " << request.getQuery() << std::endl;
	std::cout << "Body: " << request.getBody() << std::endl;
	std::cout << "----------------------------------------" << std::endl;
}

int main(void) {
	std::string getRequest = "GET /index.html?name=foo&age=20 HTTP/1.1\r\n"
							 "Host: localhost:8080\r\n"
							 "Connection: keep-alive\r\n"
							 "Content-Length: 0\r\n"
							 "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
							 "\r\n";
	testRequest(getRequest);

	std::string postRequest = "POST /index.html?name=foo&age=20 HTTP/1.1\r\n"
							  "Host: localhost:8080\r\n"
							  "Connection: keep-alive\r\n"
							  "Content-Length: 11\r\n"
							  "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
							  "\r\n"
							  "Hello World";
	testRequest(postRequest);

	std::string deleteRequest = "DELETE /index.html?name=foo&age=20 HTTP/1.1\r\n"
								"Host: localhost:8080\r\n"
								"Connection: close\r\n"
								"Content-Length: 0\r\n"
								"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n"
								"\r\n";
	testRequest(deleteRequest);

	std::string bodyRequest = "POST /submit HTTP/1.1\r\n"
							  "Host: localhost:8080\r\n"
							  "Connection: keep-alive\r\n"
							  "Content-Length: 27\r\n"
							  "Accept: */*\r\n"
							  "Content-Type: application/x-www-form-urlencoded\r\n"
							  "\r\n"
							  "field1=value1&field2=value2";
	testRequest(bodyRequest);

	// New request to test different content types
	std::string jsonRequest = "POST /api/data HTTP/1.1\r\n"
							  "Host: localhost:8080\r\n"
							  "Connection: keep-alive\r\n"
							  "Content-Length: 18\r\n"
							  "Accept: application/json\r\n"
							  "Content-Type: application/json\r\n"
							  "\r\n"
							  "{\"key\":\"value\"}";
	testRequest(jsonRequest);

	return 0;
}