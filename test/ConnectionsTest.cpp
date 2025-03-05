#include "Connections.hpp"
#include <iostream>

int main() {
	Connections connections;

	std::cout << connections.hasConnection(5) << std::endl;
	connections.addConnection(5);
	std::cout << connections.hasConnection(5) << std::endl;

	try {
		connections.addConnection(5);
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	std::cout << "removeConnection" << std::endl;
	connections.removeConnection(5);
	std::cout << connections.hasConnection(5) << std::endl;


	std::cout << "Request handling" << std::endl;
	connections.addConnection(5);
	connections.appendRequestData(5, "GET / HTTP/1.1\r\n", 16);
	connections.appendRequestData(5, "Host: localhost:8080\r\n", 21);
	connections.appendRequestData(5, "\r\n", 2);
	connections.appendRequestData(5, "\r\n", 2);
	connections.appendRequestData(5, "GET / HTTP/1.1\r\n", 16);
	connections.appendRequestData(5, "Host: localhost:8080/api\r\n", 24);
	connections.appendRequestData(5, "\r\n", 2);
	connections.appendRequestData(5, "\r\n", 2);
	std::cout << connections.hasRequest(5) << std::endl;

	std::cout << "getRequest1" << std::endl;
	std::cout << connections.getRequest(5) << std::endl;
	std::cout << connections.hasRequest(5) << std::endl;
	
	std::cout << "getRequest2" << std::endl;
	std::cout << connections.getRequest(5) << std::endl;
	std::cout << connections.hasRequest(5) << std::endl;

	std::cout << "getRequest3" << std::endl;
	try {
		std::cout << connections.getRequest(5) << std::endl;
	} catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}