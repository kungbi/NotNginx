#include "Connections.hpp"

#include <iostream>

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
	// Ensure the connection exists before deleting
	if (this->connections_.find(fd) == this->connections_.end()) {
		throw std::runtime_error("Attempting to remove a non-existent connection");
	}

	delete this->connections_.at(fd); // Free the memory
	this->connections_.erase(fd);    // Remove the entry from the map
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

Connections::~Connections() {
	std::map<int, Connection*>::iterator it;
	for (it = connections_.begin(); it != connections_.end(); ++it) {
		delete it->second; // Ensure all dynamically allocated memory is freed
	}
	connections_.clear();
}

std::string Connections::getRequest(int fd) {
	return this->connections_.at(fd)->getRequest();
}

size_t Connections::getOriginalRequestSize(int fd) const {
	return this->connections_.at(fd)->getOriginalRequestSize();
}

size_t Connections::getBodySize(int fd) const {
	return this->connections_.at(fd)->getBodySize();
}

void Connections::appendCgiBuffer(int fd, int pipeFd, const std::string& data, bool isEnd) {
	this->connections_.at(fd)->appendCgiBuffer(pipeFd, data, isEnd);
}