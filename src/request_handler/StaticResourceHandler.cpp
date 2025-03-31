#include "StaticResourceHandler.hpp"
#include <iostream>

Response* StaticResourceHandler::execute(std::string path, std::string fileExtension) {
	std::cout << "path: " << path << std::endl;
	std::cout << "fileExtension: " << fileExtension << std::endl;
	std::ifstream file(path);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file");
	}

	std::string body, line;
	while (std::getline(file, line)) {
		body += line + "\n";
	}
	file.close();

	StaticResourceResponse* response = StaticResourceResponse::Builder()
			.setProtocolVersion("HTTP/1.1")
			.setStatusCode(200)
			.setReasonPhrase("OK")
			.setServer("Server")
			.setContentType(
				getContentType(fileExtension)
			)
			.setConnection("close")
			.setBody(body)
			.build();

	return response;
}