#include "Servers.hpp"

Servers::Servers(Kqueue& kqueue): kqueue_(kqueue) {}

Servers::~Servers() {
	for (size_t i = 0; i < servers_.size(); ++i) {
		delete servers_[i]; // Ensure all dynamically allocated servers are freed
	}
	servers_.clear();
}

void Servers::updateLastActiveTime(int serverFd, int clientFd) {
	std::cout << "Updating last active time for server FD: " << serverFd << ", client FD: " << clientFd << std::endl;
	Server* server = getServerForSocketFd(serverFd);
	if (server == nullptr) {
		throw std::runtime_error("No server found for server FD");
	}
	server->updateLastActiveTime(clientFd);
}

Server* Servers::createServer(Socket& socket, ServerConfig& config, Kqueue& kqueue, RequestHandler& requestHandler) {
	std::cout << "Creating server for: " << config.getServerName() << ":" << config.getPort() << std::endl;
	return new Server(socket, config, kqueue, requestHandler);
}

void Servers::addServer(Server &server) {
	std::cout << "Adding server with FD: " << server.getSocketFd() << std::endl;
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
	std::cerr << "No server found for FD: " << fd << std::endl;
	return nullptr;
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
	return result;
}

int Servers::processResponse(int serverFd, int clientFd) {
	Server* server = getServerForSocketFd(serverFd);
	if (!server) {
		throw std::runtime_error("No server found for client FD");
	}

	server->handleResponse(clientFd);
	kqueue_.removeEvent(clientFd, EVFILT_WRITE);
	return 0;
}

int Servers::processCgiResponse(int serverFd, int clientFd, int cgiFd) {
	Server* server = getServerForSocketFd(serverFd);
	if (!server) {
		std::cerr << "No server found for CGI FD: " << serverFd << std::endl;
		return 0;
	}

	int result = server->handleCgiResponse(cgiFd, clientFd);
	return result;
}
