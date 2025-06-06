#include "Connections.hpp"

#include <iostream>

Connections::Connections() {}

void Connections::appendRequestData(int fd, const char* data, size_t length) {
	if (this->connections_.find(fd) == this->connections_.end()) {
		return;
	}
	this->connections_.at(fd)->appendRequestData(data, length);
}

std::vector<int> Connections::validateLastActiveTime(int timeoutMs) {
	time_t now = time(NULL);
	std::vector<int> timedOutFds;

	std::map<int, Connection*>::iterator it = this->connections_.begin();
	while (it != this->connections_.end()) {
		if (it->second->isTimeout(now, timeoutMs) == true) {
			timedOutFds.push_back(it->first);
		}
		++it;
	}

	for (size_t i = 0; i < timedOutFds.size(); ++i) {
		this->removeConnection(timedOutFds[i]);
	}
	return timedOutFds;
}

void Connections::updateLastActiveTime(int fd) {
	if (this->connections_.find(fd) == this->connections_.end()) {
		return;
	}
	this->connections_.at(fd)->updateLastActiveTime();
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
		return ;
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
	if (this->connections_.find(fd) == this->connections_.end()) {
		return;
	}
	this->connections_.at(fd)->appendCgiBuffer(pipeFd, data, isEnd);
}