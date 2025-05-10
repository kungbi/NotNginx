#include "Socket.hpp"

void Socket::setSocketOptions() {
	int opt = 1;
	if (setsockopt(socketFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		perror("Set socket options failed");
		closeSocket();
		exit(EXIT_FAILURE);
	}
}

void Socket::bindSocket() {
	if (bind(socketFd_, reinterpret_cast<struct sockaddr*>(&address_), sizeof(address_)) == -1) {
		perror("Socket bind failed");
		closeSocket();
		exit(EXIT_FAILURE);
	}
}

void Socket::listenSocket(int backlog = 5) {
	if (listen(socketFd_, backlog) == -1) {
		perror("Socket listen failed");
		closeSocket();
		exit(EXIT_FAILURE);
	}
}

void Socket::configureSocket(const std::string& ip, int port) {
	std::memset(&address_, 0, sizeof(address_));
	address_.sin_family = AF_INET;
	address_.sin_port = htons(port);
	address_.sin_addr.s_addr = inet_addr(ip.c_str());
}

Socket::Socket(const std::string& ip, int port) {
	// 소켓 생성
	socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFd_ == -1) {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	// 소켓 설정 및 상태 전환
	configureSocket(ip, port);
	setSocketOptions();
	bindSocket();
	listenSocket(1024);
	std::cout << "Socket is now in listening state at " << ip << ":" << port << std::endl;
}

Socket::~Socket() {
	closeSocket();
}

int Socket::acceptConnection() {
	socklen_t addrLen = sizeof(address_);
	int clientFd = accept(socketFd_, reinterpret_cast<struct sockaddr*>(&address_), &addrLen);
	if (clientFd == -1) {
		perror("Socket accept failed");
	} else {
		std::cout << "Accepted connection with FD: " << clientFd << std::endl;
	}
	return clientFd;
}

int Socket::getSocketFd() const {
	return socketFd_;
}

void Socket::closeSocket() {
	if (socketFd_ != -1) {
		close(socketFd_);
		socketFd_ = -1;
		std::cout << "Socket closed." << std::endl;
	}
}
