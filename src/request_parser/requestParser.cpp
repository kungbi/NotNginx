#include "RequestParser.hpp"
#include <sstream>
#include <iostream>
#include <iterator>

Request RequestParser::parseRequestHeader(const std::string& originalRequest) {
	if (originalRequest.empty()) {
		throw std::invalid_argument("originalRequest is empty!");
	}

	std::istringstream stream(originalRequest);
	std::string line;

	// 1️⃣ 요청 라인 파싱
	if (!std::getline(stream, line) || line.empty()) {
		throw std::invalid_argument("Invalid request line");
	}
	if (line.back() == '\r') line.pop_back();

	std::istringstream lineStream(line);
	std::string method, target, version;
	if (!(lineStream >> method >> target >> version)) {
		throw std::invalid_argument("Invalid request line");
	}

	UriComponents uri = parseUri(target);

	// 2️⃣ 헤더 파싱
	HeaderResult headerResult = parseHeaders(stream);

	// 3️⃣ 바디 파싱
	std::string body = parseBody(stream, headerResult.contentLength);

	RequestType requestType =
		(method == "GET") ? GET :
		(method == "POST") ? POST :
		(method == "PUT") ? PUT :
		(method == "PATCH") ? PATCH : DELETE;

	return Request(requestType, version, headerResult.host, target, uri.query, 
				   uri.filename, uri.extension, uri.path, headerResult.port, 
				   headerResult.connection, headerResult.contentLength, 
				   headerResult.accept, body, headerResult.contentType);
}

HeaderResult RequestParser::parseHeaders(std::istringstream& stream) {
	HeaderResult result;
	std::string line;

	memset(&result, 0, sizeof(HeaderResult));

	while (std::getline(stream, line)) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}

		if (line.empty()) break; // 빈 줄이면 헤더 종료

		size_t colonPos = line.find(':');
		if (colonPos == std::string::npos) {
			std::cerr << "Warning: Invalid header format: " << line << std::endl;
			continue;
		}

		std::string key = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1);

		while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) {
			value = value.substr(1);
		}

		if (key == "Host") {
			size_t portPos = value.find(':');
			result.host = (portPos != std::string::npos) ? value.substr(0, portPos) : value;
			result.port = (portPos != std::string::npos) ? std::stoi(value.substr(portPos + 1)) : 0;
		} else if (key == "Connection") {
			result.connection = value;
		} else if (key == "Content-Length") {
			std::cout << "contentLength: " << value << std::endl;
			std::cout << "contentLength: " << std::stoi(value) << std::endl;
			result.contentLength = std::stoi(value);
		} else if (key == "Accept") {
			result.accept = value;
		} else if (key == "Content-Type") {
			result.contentType = value;
		}
	}
	return result;
}

std::string RequestParser::parseBody(std::istringstream& stream, size_t contentLength) {
	std::cout << "contentLength: " << contentLength << std::endl;
	if (contentLength == 0) return "";

	std::string body(contentLength, '\0');
	stream.read(&body[0], contentLength);
	return body;
}

UriComponents RequestParser::parseUri(const std::string& target) {
	UriComponents result;
	size_t queryPos = target.find("?");
	result.path = (queryPos != std::string::npos) ? target.substr(0, queryPos) : target;
	result.query = (queryPos != std::string::npos) ? target.substr(queryPos + 1) : "";

	size_t lastSlash = result.path.find_last_of("/");
	if (lastSlash != std::string::npos) {
		std::string_view lastSegment(result.path.c_str() + lastSlash + 1);
		size_t dotPos = lastSegment.find_last_of(".");

		if (!lastSegment.empty() && std::all_of(lastSegment.begin(), lastSegment.end(), ::isdigit)) {
			result.filename = lastSegment;
			result.path = result.path.substr(0, lastSlash);
		} else if (dotPos != std::string::npos) {
			result.filename = std::string(lastSegment);
			result.extension = std::string(lastSegment.substr(dotPos));
			result.path = result.path.substr(0, lastSlash);
		}
	}
	return result;
}

