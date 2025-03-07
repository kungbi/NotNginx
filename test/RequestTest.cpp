// #include "Request.hpp"
#include "../include/webserver/Request.hpp"
#include "../include/request_parser/requestParser.hpp"

void test(Request *request)
{
	std::cout << "Type : ";
	switch (request->getRequestType()) {
		case GET:    std::cout << "GET"; break;
		case POST:   std::cout << "POST"; break;
		case PUT:    std::cout << "PUT"; break;
		case PATCH:  std::cout << "PATCH"; break;
		case DELETE: std::cout << "DELETE"; break;
	}
	std::cout << std::endl;
	std::cout << "protocolVersion : " << request->getProtocolVersion() << std::endl;
	std::cout << "host : " << request->getHost() << std::endl;
	std::cout << "target : " << request->getTarget() << std::endl;
	std::cout << "query : " << request->getQuery() << std::endl;
	std::cout << "path : " << request->getPath() << std::endl;
	std::cout << "fileName : " << request->getFilename() << std::endl;
	std::cout << "extension : " << request->getExtension() << std::endl;
	std::cout << "port : " << request->getPort() << std::endl;
	std::cout << "connection : " << request->getConnection() << std::endl;
	std::cout << "accept : " << request->getAccept() << std::endl;
	std::cout << "content-length : " << request->getContentLength() << std::endl;
}

int main() {
	Request request(5);
	request.appendData("GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n", 40);
	std::cout << "Request complete: " << request.isComplete() << std::endl;


	Request request2(6);
	request2.appendData("GET / HTTP/1.1\r\nHost: localhost:8080", 36);
	std::cout << "Request complete: " << request2.isComplete() << std::endl;

	std::cout<<"======Original Request======" <<std::endl;
	std::cout<<request2.getOriginalRequest()<<std::endl;
	std::cout<<"============================" << std::endl;

	RequestParser parser;
	parser.parseRequestHeader(&request2);
	
	test(&request2);

	return 0;
}
