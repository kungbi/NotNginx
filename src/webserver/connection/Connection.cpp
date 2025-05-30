#include "Connection.hpp"

Connection::Connection(void) {}

Connection::~Connection(void) {}

Connection::Connection(int fd): fd_(fd), lastActiveTime_(time(NULL)) {
	(void) fd_;
}

bool Connection::isTimeout(const time_t currentTime,  const int timeoutMs) const {
	if (difftime(currentTime, this->lastActiveTime_) > timeoutMs / 1000) {
		return true; // 타임아웃
	}
	return false;
}

void Connection::updateLastActiveTime(void) {
	this->lastActiveTime_ = time(NULL);
}

void Connection::appendRequestData(const char* data, size_t length) {
	this->originalRequest_.append(data, length);
}

bool Connection::hasRequest(void) const {
	std::cout << "originalRequest_: " << this->originalRequest_ << std::endl;
    size_t headerEnd = this->originalRequest_.find("\r\n\r\n");
	std::cout << "headerEnd: " << headerEnd << std::endl;
    if (headerEnd == std::string::npos)
        return false; // 헤더도 아직 다 안 옴

    size_t contentLengthPos = this->originalRequest_.find("Content-Length:");
    if (contentLengthPos != std::string::npos) {
        // Content-Length가 있으면
        size_t contentStart = headerEnd + 4;
        // Content-Length 값을 읽어오기
        size_t lengthStart = contentLengthPos + std::string("Content-Length:").length();
        size_t lengthEnd = this->originalRequest_.find("\r\n", lengthStart);
        std::string lengthStr = this->originalRequest_.substr(lengthStart, lengthEnd - lengthStart);
        size_t contentLength = std::atoi(lengthStr.c_str());

        return (this->originalRequest_.length() >= contentStart + contentLength);
    }

    // Content-Length 없으면 헤더까지만 받으면 됨
    return true;
}

size_t Connection::getOriginalRequestSize(void) const {
	return this->originalRequest_.size();
}

size_t Connection::getBodySize(void) const {
    size_t headerEnd = this->originalRequest_.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        // 헤더 끝을 못 찾았으면 body도 아직 없음
        return 0;
    }

    size_t contentLengthPos = this->originalRequest_.find("Content-Length:");
    if (contentLengthPos == std::string::npos) {
        // Content-Length 헤더가 없으면 body가 없다고 가정
        return 0;
    }

    size_t lengthStart = contentLengthPos + std::string("Content-Length:").length();

    // Content-Length 값만 깔끔히 추출
    while (lengthStart < this->originalRequest_.size() && (this->originalRequest_[lengthStart] == ' ' || this->originalRequest_[lengthStart] == '\t')) {
        ++lengthStart; // 공백 스킵
    }

    size_t lengthEnd = this->originalRequest_.find("\r\n", lengthStart);
    if (lengthEnd == std::string::npos) {
        return 0; // 잘못된 헤더
    }

    std::string lengthStr = this->originalRequest_.substr(lengthStart, lengthEnd - lengthStart);

    // 숫자로 변환
    int contentLength = std::atoi(lengthStr.c_str());
    if (contentLength < 0) {
        return 0; // 음수면 잘못된 Content-Length
    }

    return static_cast<size_t>(contentLength);
}

std::string Connection::getRequest(void) {
	if (!this->hasRequest()) {
		throw std::runtime_error("No request available");
	}

	size_t headerEnd = this->originalRequest_.find("\r\n\r\n");
	if (headerEnd == std::string::npos) {
		throw std::runtime_error("No header end found");
	}

	size_t bodyStart = headerEnd + 4;
	size_t totalRequestSize = bodyStart;

	size_t contentLengthPos = this->originalRequest_.find("Content-Length:");
	if (contentLengthPos != std::string::npos) {
		size_t lengthStart = contentLengthPos + std::string("Content-Length:").length();
		while (lengthStart < this->originalRequest_.size() && (this->originalRequest_[lengthStart] == ' ' || this->originalRequest_[lengthStart] == '\t')) {
			++lengthStart;
		}
		size_t lengthEnd = this->originalRequest_.find("\r\n", lengthStart);
		if (lengthEnd == std::string::npos) {
			throw std::runtime_error("Invalid Content-Length header");
		}

		std::string lengthStr = this->originalRequest_.substr(lengthStart, lengthEnd - lengthStart);
		int contentLength = std::atoi(lengthStr.c_str());
		if (contentLength < 0) {
			throw std::runtime_error("Invalid Content-Length value");
		}

		totalRequestSize += static_cast<size_t>(contentLength);
	}

	// 이제 헤더 + body를 합친 걸 리턴
	std::string request = this->originalRequest_.substr(0, totalRequestSize);
	this->originalRequest_ = this->originalRequest_.substr(totalRequestSize);
	return request;
}


void Connection::addResponse(Response& response) {
	this->responses_.addResponse(response);
}

bool Connection::hasResponse(void) const {
	return this->responses_.hasResponse();
}

Response* Connection::getResponse(void) {
	if (!this->hasResponse()) {
		throw std::runtime_error("No response available");
	}

	return this->responses_.popResponse();
}

void Connection::appendCgiBuffer(int pipeFd, const std::string& data, bool isEnd) {
	this->responses_.appendCgiBuffer(pipeFd, data, isEnd);
}