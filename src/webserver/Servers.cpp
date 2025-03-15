#include "Servers.hpp"

Servers::Servers(Kqueue& kqueue): kqueue_(kqueue) {}

Servers::~Servers() {
	for (size_t i = 0; i < servers_.size(); ++i) {
		delete servers_[i]; // 서버 객체 삭제
	}
}

Server* Servers::createServer(Socket& socket, ServerConfig& config, Kqueue& kqueue) {
	return new Server(socket, config, kqueue);
}

void Servers::addServer(Server &server) {
	servers_.push_back(&server);
}

bool Servers::isServerSocketFd(int fd) {
	std::cout << "Checking for FD: " << fd << std::endl;

	for (size_t i = 0; i < servers_.size(); ++i) {
		if (servers_[i]->getSocketFd() == fd) {
			return true;
		}
	}
	return false;
}

Server* Servers::getServerForSocketFd(int fd) {
	std::cout << "Getting server for FD: " << fd << std::endl;
	for (size_t i = 0; i < servers_.size(); ++i) {
		if (servers_[i]->getSocketFd() == fd) {
			std::cout << "Get Server FD: " << servers_[i]->getSocketFd() << std::endl;
			return servers_[i];
		}
	}
	return nullptr;
}

void Servers::handleRequest(int fd) {
	const char* response = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Length: 12\r\n"
		"Connection: close\r\n"
		"\r\n"
		"Hello World\n";

	write(fd, response, strlen(response));
}

size_t Servers::size() const {
	return servers_.size();
}

Server& Servers::getServer(size_t index) {
	if (index >= servers_.size()) {
		throw std::out_of_range("Server index out of range.");
	}
	return *servers_[index];
}

int Servers::connectClient(int serverFd) {
	Server* server = getServerForSocketFd(serverFd);
	if (server == nullptr) {
		throw std::runtime_error("No server found for server FD");
	}

	int clientFd = server->acceptClient();
	if (clientFd == -1) {
		throw std::runtime_error("Failed to accept client");
	}
	return clientFd;
}

int Servers::processRequest(int serverFd, int clientFd) {
	Server* server = getServerForSocketFd(serverFd);
	if (!server) {
		throw std::runtime_error("No server found for client FD");
	}

	int result = server->handleRequest(clientFd);
	std::cout << "Result: " << result << std::endl;
	if (result == 0) {
		kqueue_.removeEvent(clientFd, EVFILT_READ);
		// server->closeConnection(clientFd);
	}

	return result;
}

int Servers::processResponse(int serverFd, int clientFd) {
	Server* server = getServerForSocketFd(serverFd);
	if (!server) {
		throw std::runtime_error("No server found for client FD");
	}

	server->handleResponse(clientFd);
	server->closeConnection(clientFd);
	return 0;
}