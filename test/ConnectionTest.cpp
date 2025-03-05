#include "Connection.hpp"
#include "Response.hpp"
#include <iostream>

int main() {
	Connection conn(5);

	std::cout << conn.hasRequest() << std::endl;
	conn.appendRequestData("GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n", 40);

	std::cout << conn.hasRequest() << std::endl;
	conn.appendRequestData("GET / HTTP/1.1\r\nHost: localhost:8080\r\n\r\n", 40);

	std::cout << conn.getRequest() << std::endl;
	std::cout << conn.getRequest() << std::endl;

	std::cout << conn.hasResponse() << std::endl;
	conn.addResponse(Response::Builder()
		.setProtocolVersion("HTTP/1.1")
		.setStatusCode(200)
		.setReasonPhrase("OK")
		.setServer("Server")
		.setContentType("text/html")
		.setConnection("close")
		.setBody(
			"<html>\n" 
			"<body>\n" 
				"<h1>Welcome to our website</h1>\n" 
			"</body>\n" 
			"</html>"
		)
		.build()
	);

	std::cout << conn.hasResponse() << std::endl;
	Response response = conn.getResponse();
	std::cout << response.getResponse() << std::endl;
	std::cout << conn.hasResponse() << std::endl;
	

	return 0;
}