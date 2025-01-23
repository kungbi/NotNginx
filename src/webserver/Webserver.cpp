#include "Webserver.hpp"

void Webserver::initializeServers(const WebserverConfig& config) {
	HTTPConfig& httpConfig = config.getHTTPConfig();

	for (std::vector<ServerConfig>::const_iterator it = httpConfig.getServers().begin(); it != httpConfig.getServers().end(); ++it) {
		ServerConfig serverConfig = *it;
		Server* server = servers.createServer(serverConfig);

		kqueueManager.addEvent(server->getSocketFd(), SERVER, *server); // Kqueue에 등록
		servers.addServer(*server);
	}
}


void Webserver::handleServerSocketEvent(int fd) {
	Server* server = servers.getServerForSocketFd(fd);
	if (server) {
		int clientFd = server->acceptClient(); // 클라이언트 FD 반환
		if (clientFd != -1) {
			kqueueManager.addEvent(clientFd, REQUEST, *server); // Kqueue에 등록
			std::cout << "Client FD: " << clientFd << " registered with Kqueue." << std::endl;
		} else {
			std::cerr << "Failed to accept client on FD: " << fd << std::endl;
		}
	} else {
		std::cerr << "No server found for server socket FD: " << fd << std::endl;
	}
}

void Webserver::handleClientRequest(int fd) {
	servers.handleRequest(fd);
	
	kqueueManager.removeFd(fd, EVFILT_READ);
	close(fd);
	std::cout << "Connection closed for FD: " << fd << std::endl;
}

void Webserver::processEvents(struct kevent& event) {
	int fd = event.ident;
	int filter = event.filter;

	if (filter == EVFILT_READ) {
		processReadEvent(fd); // READ 이벤트 처리
	}
}

void Webserver::processReadEvent(int fd) {
	if (servers.isServerSocketFd(fd)) {
		handleServerSocketEvent(fd); // 서버 소켓 이벤트 처리
	} else {
		// 클라이언트 요청 처리
		char buffer[1024];
		ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);

		if (bytesRead > 0) {
			buffer[bytesRead] = '\0'; // Null-terminate for safety
			std::cout << "Received: " << buffer << " from FD: " << fd << std::endl;

			handleClientRequest(fd); // 요청 처리 (예: HTTP 응답)
		} else if (bytesRead == 0) {
			// 클라이언트가 연결을 닫은 경우
			std::cout << "Client disconnected on FD: " << fd << std::endl;
			kqueueManager.removeFd(fd, EVFILT_READ); // Kqueue에서 제거
			close(fd); // 소켓 닫기
		} else {
			// 읽기 실패 (에러 처리)
			perror("Error reading from FD");
			kqueueManager.removeFd(fd, EVFILT_READ); // Kqueue에서 제거
			close(fd); // 소켓 닫기
		}
	}
}

Webserver::Webserver(const WebserverConfig& config): kqueueManager(config.getWorkerConnections()) {
	initializeServers(config);
}

void Webserver::start() {
	std::cout << "Webserver started." << std::endl;

	while (true) {
		struct kevent* event = kqueueManager.pollEvents();
		processEvents(*event); // 이벤트 처리
		delete event; // 메모리 해제
	}
}
