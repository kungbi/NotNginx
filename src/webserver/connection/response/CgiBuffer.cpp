#include "CgiBuffer.hpp"

void CgiBuffer::appendBuffer(int fd, const std::string& data) {
	buffer_[fd].append(data);
}

std::string CgiBuffer::popBuffer(int fd) {
	if (this->hasBuffer(fd) == false)
		throw std::runtime_error("No buffer for this fd");

	std::string data = buffer_[fd];
	buffer_.erase(fd);
	return data;
}

bool CgiBuffer::hasBuffer(int fd) {
	if (buffer_.find(fd) == buffer_.end())
		return false;
	return true;
}

