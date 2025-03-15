#include "Connection.hpp"

Connection::Connection(void) {}

Connection::~Connection(void) {}

Connection::Connection(int fd): fd_(fd) {
	(void) fd_;
}

void Connection::appendRequestData(const char* data, size_t length) {
	this->originalRequest_.append(data, length);
}

bool Connection::hasRequest(void) const {
	return this->originalRequest_.find("\r\n\r\n") != std::string::npos;
}

std::string Connection::getRequest(void) {
	if (!this->hasRequest()) {
		throw std::runtime_error("No request available");
	}

	size_t pos = this->originalRequest_.find("\r\n\r\n");
	std::string request = this->originalRequest_.substr(0, pos + 4);
	this->originalRequest_ = this->originalRequest_.substr(pos + 4);
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