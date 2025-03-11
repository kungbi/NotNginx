#include "CgiBuffer.hpp"
#include <iostream>

int main() {
	CgiBuffer buffer;

	std::cout << buffer.hasBuffer(1) << std::endl;
	buffer.appendBuffer(1, "Hello");
	std::cout << buffer.hasBuffer(1) << std::endl;
	std::cout << buffer.popBuffer(1) << std::endl;
	std::cout << buffer.hasBuffer(1) << std::endl;
	return 0;
}