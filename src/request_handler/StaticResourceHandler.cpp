#include "StaticResourceHandler.hpp"
#include <iostream>
#include <dirent.h>

Response* StaticResourceHandler::execute(std::string path, std::string fileExtension) {
	std::cout << "path: " << path << std::endl;
	std::cout << "fileExtension: " << fileExtension << std::endl;

	if (fileExtension.empty()) {
		// Handle directory case
		std::string body;
		DIR* dir = opendir(path.c_str());
		if (!dir) {
			throw NotFoundError("Directory not found");
		}

		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL) {
			// Skip "." and ".."
			if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..") {
				body += entry->d_name;
				body += "\n";
			}
		}
		closedir(dir);

		StaticResourceResponse* response = StaticResourceResponse::Builder()
				.setProtocolVersion("HTTP/1.1")
				.setStatusCode(200)
				.setReasonPhrase("OK")
				.setServer("Server")
				.setContentType("text/plain")
				.setConnection("close")
				.setBody(body)
				.build();

		return response;
	}

	std::ifstream file(path);
	if (!file.is_open()) {
		throw NotFoundError("File not found");
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