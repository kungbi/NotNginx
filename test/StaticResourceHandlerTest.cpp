#include "StaticResourceHandler.hpp"
#include "StaticResourceResponse.hpp"
#include <iostream>


int main() {
	Response* response = StaticResourceHandler::execute("./test.txt", "txt"); 

	std::cout << response->getResponse() << std::endl;
	return 0;
}