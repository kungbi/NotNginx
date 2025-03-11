#include <iostream>
#include "Responses.hpp"
#include "StaticResourceResponse.hpp"

int main() {
	Responses responses;

	StaticResourceResponse response = StaticResourceResponse::Builder()
		.setProtocolVersion("HTTP/1.1")
		.setStatusCode(200)
		.setReasonPhrase("OK")
		.setServer("Server")
		.setContentType("text/html")
		.setConnection("close")
		.setBody(
			"\n" 
			"<html>\n" 
			"<body>\n" 
				"<h1>Welcome to our website</h1>\n" 
			"</body>\n" 
			"</html>"
		)
		.build();

	std::cout << responses.hasResponse() << std::endl;
	responses.addResponse(response);
	std::cout << responses.hasResponse() << std::endl;

	Response* returnResponse =  responses.popResponse();
	std::cout << returnResponse->getResponse() << std::endl;

	std::cout << "======================" << std::endl;
	
	std::cout << "hasResponse: " << responses.hasResponse() << std::endl;
	responses.appendCgiResponse(
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 10\r\n"
		"\r\n"
		"Hello World"
	);
	std::cout << "hasResponse: " << responses.hasResponse() << std::endl;
	std::cout << responses.popResponse()->getResponse() << std::endl;
	std::cout << "hasResponse: " << responses.hasResponse() << std::endl;



	
	return 0;
}