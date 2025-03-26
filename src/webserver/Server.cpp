#include "Server.hpp"

std::string requestTypeToString(RequestType type) {
	switch (type) {
		case GET: return "GET";
		case POST: return "POST";
		case PUT: return "PUT";
		case PATCH: return "PATCH";
		case DELETE: return "DELETE";
		default: return "UNKNOWN";
	}
}

Server::Server(Socket& serverSocket, ServerConfig& serverConfig, Kqueue& kqueue)
	: serverSocket_(serverSocket),
	  serverConfig_(serverConfig),
	  kqueue_(kqueue),
	  router_(serverConfig),
	  cgiHandler_(kqueue),
	  requestHandler_(router_, cgiHandler_) {
	std::cout << "Server initialized at " << serverConfig.getServerName() << ":" << serverConfig.getPort() << std::endl;
}

int Server::getSocketFd() const {
	return serverSocket_.getSocketFd();
}

int Server::acceptClient() {
	return serverSocket_.acceptConnection(); // 클라이언트 요청을 수락하고 FD 반환
}

int Server::processClientData(int clientFd, const char* buffer, ssize_t bytesRead) {
	std::cout << "Received: " << buffer << " from FD: " << clientFd << std::endl;
	if (!this->connections_.hasConnection(clientFd)) {
		this->connections_.addConnection(clientFd);
	}

	this->connections_.appendRequestData(clientFd, buffer, bytesRead);

	if (this->connections_.hasRequest(clientFd)) {
		Request request = RequestParser::parseRequestHeader(this->connections_.getRequest(clientFd));
		Response* response = requestHandler_.dispatch(request, clientFd);
		if (response != NULL) {
			this->connections_.addResponse(clientFd, *response);
			this->kqueue_.addEvent(clientFd, KQUEUE_EVENT::RESPONSE, this->getSocketFd());
			std::cout << "Response added for client FD: " << clientFd << std::endl;
		}	
		return 0;
	}

	std::cout << "No request yet" << std::endl;

	return 2;
}

void Server::sendResponse(int clientFd, const std::string& response) {
	send(clientFd, response.c_str(), response.size(), 0);
}

int Server::handleRequest(int clientFd) { // <- 함수 분리 전
	std::cout << "Handling request for client FD: " << clientFd << std::endl;

	// 클라이언트 요청 처리
	char buffer[1025];
	ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

	if (bytesRead > 0) {
		buffer[bytesRead] = '\0'; // Null-terminate for safety
		return processClientData(clientFd, buffer, bytesRead);
	}
	
	if (bytesRead == 0) {
		// 클라이언트가 연결을 닫은 경우
		std::cout << "Client disconnected on FD: " << clientFd << std::endl;
		close(clientFd); // 소켓 닫기
		return 1;
	}
	
	perror("Error reading from FD");
	close(clientFd); // 소켓 닫기
	return 1;
}

int Server::handleResponse(int clientFd) {
	std::cout << "Handling response for client FD: " << clientFd << std::endl;
	if (!this->connections_.hasResponse(clientFd)) {
		std::cout << "No response found for client FD: " << clientFd << std::endl;
		throw std::runtime_error("No response found for client FD");
	}

	Response* response = this->connections_.getResponse(clientFd);
	sendResponse(clientFd, response->getResponse());
	return 0;
}

void Server::closeConnection(int clientFd) {
	close(clientFd);
	this->connections_.removeConnection(clientFd);
}