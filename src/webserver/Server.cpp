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

Server::Server(Socket& serverSocket, ServerConfig& serverConfig, Kqueue& kqueue, RequestHandler& requestHandler)
	: serverSocket_(serverSocket),
	  serverConfig_(serverConfig),
	  kqueue_(kqueue),
	  requestHandler_(requestHandler) {
	std::cout << "Server initialized at " << serverConfig.getServerName() << ":" << serverConfig.getPort() << std::endl;
}

int Server::getSocketFd() const {
	return serverSocket_.getSocketFd();
}

int Server::acceptClient() {
	int clientFd = serverSocket_.acceptConnection();
	this->connections_.addConnection(clientFd);
	return clientFd;
}

std::vector<int> Server::validateLastActiveTime(int timeoutMs) {
	std::vector<int> timedOutFds = this->connections_.validateLastActiveTime(timeoutMs);
	for (size_t i = 0; i < timedOutFds.size(); ++i) {
		this->handleError(timedOutFds[i], 408); // Request Timeout
	}
	return timedOutFds;
}

void Server::updateLastActiveTime(int clientFd) {
	this->connections_.updateLastActiveTime(clientFd);
}

int Server::processClientData(int clientFd, const char* buffer, ssize_t bytesRead) {
	std::cout << "Received: " << buffer << " from FD: " << clientFd << std::endl;
	if (!this->connections_.hasConnection(clientFd)) {
		this->connections_.addConnection(clientFd);
	}

	this->connections_.appendRequestData(clientFd, buffer, bytesRead);

	if (this->serverConfig_.getClientMaxBodySize() < this->connections_.getBodySize(clientFd)) {
		throw RequestEntityTooLargeError("Request size exceeds max body size");
	}

	if (this->connections_.hasRequest(clientFd)) {
		Request request = RequestParser::parseRequestHeader(this->connections_.getRequest(clientFd));
		Response* response = requestHandler_.dispatch(request, clientFd);
		if (response != NULL) {
			this->connections_.addResponse(clientFd, *response);
			this->kqueue_.addEvent(clientFd, KQUEUE_EVENT::RESPONSE, this->getSocketFd());
			std::cout << "Response added for client FD: " << clientFd << std::endl;
		}	
		return SUCCESS;
	}

	std::cout << "No request yet" << std::endl;
	return NO_REQUEST_YET;
}

bool Server::sendResponse(int clientFd, const std::string& response) {
	ssize_t bytesSent = send(clientFd, response.c_str(), response.size(), 0);
	if (bytesSent == -1) {
		closeConnection(clientFd);
		return false;
	}
	return true;
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
		this->closeConnection(clientFd); // 소켓 닫기
		return CLIENT_DISCONNECTED;
	}
	
	// this->kqueue_.removeEvent(clientFd, EVFILT_READ); // 클라이언트 FD에서 이벤트 제거
	// this->closeConnection(clientFd); // 소켓 닫기
	return CLOSE;
}

int Server::handleResponse(int clientFd) {
	std::cout << "Handling response for client FD: " << clientFd << std::endl;
	if (!this->connections_.hasConnection(clientFd)) {
		std::cout << "No connection found for client FD: " << clientFd << std::endl;
		throw InternalServerError("No connection found for client FD");
	}
	if (!this->connections_.hasResponse(clientFd)) {
		std::cout << "No response found for client FD: " << clientFd << std::endl;
		throw InternalServerError("No response found for client FD");
	}

	std::cout << "Sending response to client FD: " << clientFd << std::endl;
	Response* response = this->connections_.getResponse(clientFd);
	if (sendResponse(clientFd, response->getResponse())) {
		// closeConnection(clientFd);
	}
	delete response; // 응답 객체 삭제
	return 0;
}

int Server::handleCgiResponse(int cgiFd, int clientFd) {
	char buffer[1025];
	ssize_t bytesRead = read(cgiFd, buffer, sizeof(buffer) - 1);
	if (bytesRead == -1 && errno == EAGAIN) {
		return 1; // 다시 읽도록 유지
	}

	if (bytesRead <= 0) {
		std::cout << "CGI pipe closed." << std::endl;
		this->connections_.appendCgiBuffer(clientFd, cgiFd, "", true);
		this->kqueue_.addEvent(clientFd, KQUEUE_EVENT::RESPONSE, this->getSocketFd());
		this->kqueue_.removeEvent(cgiFd, EVFILT_READ);
		close(cgiFd);
		return 0;
	}

	// CGI 응답을 연결 객체에 저장
	buffer[bytesRead] = '\0';
	this->connections_.appendCgiBuffer(clientFd, cgiFd, std::string(buffer, bytesRead), false);
	return 1; // 아직 응답 수신 중
}


void Server::closeConnection(int clientFd) {
	shutdown(clientFd, SHUT_WR); // 쓰기 종료 알림
	close(clientFd);
	this->connections_.removeConnection(clientFd);
}

void Server::handleError(int clientFd, int errorCode) {
	std::string resolvedErrorFile = resolveErrorFile(errorCode);

	std::string responseBody;
	
	std::cout << "Resolved error file: " << resolvedErrorFile << std::endl;
	if (!resolvedErrorFile.empty()) 
		responseBody = generateDefaultErrorPage(errorCode);

	if (responseBody.empty()) {
		std::cerr << "Failed to open error file: " << resolvedErrorFile << std::endl;
		responseBody = generateDefaultErrorPage(errorCode);
	}

	Response *response = StaticResourceResponse::Builder()
			.setProtocolVersion("HTTP/1.1")
			.setStatusCode(errorCode)
			.setReasonPhrase(getReasonPhrase(errorCode))
			.setServer("Server")
			.setContentType(getContentType("html"))
			.setConnection("keep-alive")
			.setBody(responseBody)
			.build();

	if (sendResponse(clientFd, response->getResponse())) {
		// closeConnection(clientFd);
	}
	delete response; // 응답 객체 삭제
}

std::string Server::resolveErrorFile(int errorCode) {
	const std::map<int, std::string>& errorPages = serverConfig_.getErrorPages();
	if (errorPages.find(errorCode) == errorPages.end()) {
		// throw NotFoundError("Error page not found errorpage");
		return "";
	}

	std::string errorFile = errorPages.at(errorCode);
	const std::vector<LocationConfig>& locations = serverConfig_.getLocations();
	for (size_t i = 0; i < locations.size(); ++i) {
		const LocationConfig& location = locations[i];
		if (errorFile.find(location.getPattern()) == 0) {
			std::string relativePath = errorFile.substr(location.getPattern().size());
			if (!relativePath.empty() && relativePath[0] == '/') {
				relativePath = relativePath.substr(1);
			}
			return location.getRoot() + "/" + relativePath;
		}
	}
	return "";
}

std::string Server::readErrorFile(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		return "";
	}
	return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void Server::sendErrorResponse(int clientFd, int errorCode, const std::string& response) {
	std::string httpResponse = 
		"HTTP/1.1 " + std::to_string(errorCode) + " Error\r\n" +
		"Content-Length: " + std::to_string(response.size()) + "\r\n" +
		"Connection: close\r\n\r\n" +
		response;

	sendResponse(clientFd, httpResponse);
}

