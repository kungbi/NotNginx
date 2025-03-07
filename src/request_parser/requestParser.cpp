#include "RequestParser.hpp"
#include "Request.hpp"

Request RequestParser::parseRequestHeader(const std::string originalRequest)
{
	if (originalRequest.empty()) {
		throw std::invalid_argument("originalRequest is empty!");
	}

	std::istringstream stream(originalRequest);
	std::string line;

	// 1️⃣ 첫 번째 줄 (요청 라인) 파싱
	if (!std::getline(stream, line)) {
		throw std::invalid_argument("No request line found");
	}

	// \r\n 처리
	if (!line.empty() && line.back() == '\r') {
		line.pop_back();
	}

	std::istringstream lineStream(line);
	std::string method, target, version;
	
	// 모든 요소가 제대로 파싱되었는지 확인
	if (!(lineStream >> method >> target >> version)) {
		throw std::invalid_argument("Invalid request line");
	}

	UriComponents uri = RequestParser::parseUri(target);

	// 헤더 파싱을 위한 변수들
	std::string host, connection, accept;
	int port = 0;
	size_t contentLength = 0;

	// 2️⃣ 헤더 파싱
	std::string currentHeader;  // 멀티라인 헤더를 위한 변수
	
	while (std::getline(stream, line)) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back();
		}

		// 빈 줄이면 헤더 섹션 종료
		if (line.empty()) {
			break;
		}

		// 헤더 라인이 공백으로 시작하면 이전 헤더의 연속
		if (line[0] == ' ' || line[0] == '\t') {
			currentHeader += ' ' + line.substr(1);
			continue;
		}

		// 이전 헤더 처리
		if (!currentHeader.empty()) {
			RequestParser::processHeader(currentHeader, host, port, connection, contentLength, accept);
		}
		currentHeader = line;
	}

	// 마지막 헤더 처리
	if (!currentHeader.empty()) {
		RequestParser::processHeader(currentHeader, host, port, connection, contentLength, accept);
	}

	// Request 객체 생성 및 반환
	RequestType requestType = (method == "GET") ? GET : (method == "POST") ? POST : (method == "PUT") ? PUT : (method == "PATCH") ? PATCH : DELETE;
	Request request(requestType, version, host, target, uri.query, uri.filename, uri.extension, uri.path, port, connection, contentLength, accept, "");

	return request;
}

// 새로운 헤더 처리 함수
void RequestParser::processHeader(const std::string& header, std::string& host, int& port, std::string& connection, size_t& contentLength, std::string& accept) {
	size_t colonPos = header.find(':');
	if (colonPos == std::string::npos) {
		std::cerr << "Warning: Invalid header format: " << header << std::endl;
		return;
	}

	std::string key = header.substr(0, colonPos);
	std::string value = header.substr(colonPos + 1);
	
	// 앞뒤 공백 제거
	while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) {
		value = value.substr(1);
	}

	try {
		if (key == "Host") {
			size_t portPos = value.find(':');
			if (portPos != std::string::npos) {
				host = value.substr(0, portPos);
				port = std::stoi(value.substr(portPos + 1));
			} else {
				host = value;
			}
		} else if (key == "Connection") {
			connection = value;
		} else if (key == "Content-Length") {
			contentLength = std::stoi(value);
		} else if (key == "Accept") {
			accept = value;
		}
	} catch (const std::exception& e) {
		std::cerr << "Error processing header '" << key << "': " << e.what() << std::endl;
	}
}

UriComponents RequestParser::parseUri(const std::string& target) {
	UriComponents result;

	// 1 쿼리 문자열 분리
	size_t queryPos = target.find("?");
	if (queryPos != std::string::npos) {
		result.path = target.substr(0, queryPos);  // `?` 앞부분만 경로로 저장
		result.query = target.substr(queryPos + 1); // `?` 이후 문자열을 쿼리로 저장
	} else {
		result.path = target;
	}

	// 2️ 마지막 슬래시(`/`) 기준으로 파일명과 경로 분리
	size_t lastSlash = result.path.find_last_of("/");
	if (lastSlash != std::string::npos) {
		std::string lastSegment = result.path.substr(lastSlash + 1);  // `/` 뒤의 문자열
		size_t dotPos = lastSegment.find_last_of(".");

		if (!lastSegment.empty() && std::all_of(lastSegment.begin(), lastSegment.end(), ::isdigit)) {
			// 숫자로만 이루어진 경우 → 리소스 ID로 판단하여 파일명으로 처리
			result.filename = lastSegment;
			result.path = result.path.substr(0, lastSlash);  // 경로에서 파일명 부분 제거
		} else if (dotPos != std::string::npos) {
			// 확장자가 포함된 파일명 처리
			result.filename = lastSegment;
			result.extension = lastSegment.substr(dotPos);
			result.path = result.path.substr(0, lastSlash);
		} else {
			// 일반적인 경로 (파일명이 아닌 경우)
			result.filename = "";
		}
	}

	return result;
}

