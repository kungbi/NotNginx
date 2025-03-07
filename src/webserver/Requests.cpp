#include "Requests.hpp"

Requests::Requests() {}

void Requests::addRequest(Request* request) {
	if (this->isExist(request->getFd())) {
		std::cerr << "Request with fd " << request->getFd() << " already exists" << std::endl;
		throw std::runtime_error("Request already exists");
	}
	requests_[request->getFd()] = request;
}

size_t Requests::size() const {
	return requests_.size();
}

bool Requests::isExist(int fd) const {
	return requests_.find(fd) != requests_.end();
}

Request* Requests::getRequest(int fd) {
	if (!this->isExist(fd)) {
		std::cerr << "Request with fd " << fd << " does not exist" << std::endl;
		throw std::runtime_error("Request with fd does not exist");
	}
	return requests_[fd];
}

void Requests::removeRequest(int fd) {
	if (!this->isExist(fd)) {
		std::cerr << "Request with fd " << fd << " does not exist" << std::endl;
		return;
	}
	if (requests_[fd])
		delete requests_[fd];
	requests_.erase(fd);
}
