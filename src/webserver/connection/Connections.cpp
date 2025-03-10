#include "Connections.hpp"

Connections::Connections() {}

void Connections::appendRequestData(int fd, const char* data, size_t length) {
	this->connections_.at(fd)->appendRequestData(data, length);
}

void Connections::addConnection(int fd) {
	if (this->hasConnection(fd)) {
		throw std::runtime_error("Connection already exists");
	}
	this->connections_[fd] = new Connection(fd);
}

void Connections::removeConnection(int fd) {
	this->connections_.erase(fd);
}

bool Connections::hasConnection(int fd) const {
	return this->connections_.find(fd) != this->connections_.end();
}

const Connection& Connections::getConnection(int fd) const {
	return *(this->connections_.at(fd));
}

bool Connections::hasRequest(int fd) const {
	return this->getConnection(fd).hasRequest();
}

void Connections::addResponse(int fd, Response& response) {
	this->connections_.at(fd)->addResponse(response);
}

bool Connections::hasResponse(int fd) const {
	return this->getConnection(fd).hasResponse();
}

Response* Connections::getResponse(int fd) {
	return this->connections_.at(fd)->getResponse();
}

Connections::~Connections() {}

std::string Connections::getRequest(int fd) {
	return this->connections_.at(fd)->getRequest();
}