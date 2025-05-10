#include "RequestParser.hpp"
#include <sstream>
#include <iostream>
#include <iterator>


int RequestParser::stoiSafe(const std::string& s, int defaultValue = 0) {
	try {
		size_t idx;
		int value = std::stoi(s, &idx);
		if (idx != s.length()) {
			throw std::invalid_argument("Contains non-digit characters");
		}
		return value;
	} catch (const std::invalid_argument& e) {
		std::cerr << "Warning: stoiSafe - invalid argument: " << s << " (" << e.what() << ")" << std::endl;
	} catch (const std::out_of_range& e) {
		std::cerr << "Warning: stoiSafe - out of range: " << s << " (" << e.what() << ")" << std::endl;
	}
	return defaultValue;
}

Request RequestParser::parseRequestHeader(const std::string& originalRequest) {
	if (originalRequest.empty()) {
		throw std::invalid_argument("originalRequest is empty!");
	}

	// 1️⃣ 헤더와 바디를 분리
	std::cout << "originalRequest: " << originalRequest << std::endl;
	size_t headerEnd = originalRequest.find("\r\n\r\n");
	if (headerEnd == std::string::npos) {
		throw std::invalid_argument("Invalid request: missing header-body separator");
	}

	std::string headerPart = originalRequest.substr(0, headerEnd);
	std::string bodyPart = originalRequest.substr(headerEnd + 4); // "\r\n\r\n" 다음부터 body 시작
	std::cout << "Header Part: " << headerPart << std::endl;
	std::cout << "Body Part: " << bodyPart << std::endl;

	std::istringstream stream(headerPart);
	std::string line;

	// 2️⃣ 요청 라인 파싱
	if (!std::getline(stream, line) || line.empty()) {
		throw BadRequestError("Invalid request line");
	}
	if (line.back() == '\r') line.pop_back();

	std::istringstream lineStream(line);
	std::string method, target, version;
	if (!(lineStream >> method >> target >> version)) {
		throw BadRequestError("Invalid request line");
	}

	UriComponents uri = parseUri(target);

	// 3️⃣ 헤더 파싱
	HeaderResult headerResult = parseHeaders(stream);

	// 4️⃣ body는 따로 잘라온 bodyPart 그대로 사용
	std::string body = bodyPart;
	if (body.size() > headerResult.contentLength) {
		body.resize(headerResult.contentLength); // 혹시 남아있으면 자르기
	}

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
			result.port = (portPos != std::string::npos) ? stoiSafe(value.substr(portPos + 1), 0) : 0;
		} else if (key == "Connection") {
			result.connection = value;
		} else if (key == "Content-Length") {
			result.contentLength = stoiSafe(value, 0);
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

