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
		std::string requestDetails = 
			"Method: " + requestTypeToString(request.getRequestType()) + "\n" +
			"Target: " + request.getTarget() + "\n" +
			"Version: " + request.getProtocolVersion() + "\n" +
			"Host: " + request.getHost() + "\n" +
			"Port: " + std::to_string(request.getPort()) + "\n" +
			"Connection: " + request.getConnection() + "\n" +
			"Content-Length: " + std::to_string(request.getContentLength()) + "\n" +
			"Accept: " + request.getAccept() + "\n" +
			"Content-Type: " + request.getContentType() + "\n" +
			"Query: " + request.getQuery() + "\n" +
			"Filename: " + request.getFilename() + "\n" +
			"Extension: " + request.getExtension() + "\n" +
			"Path: " + request.getPath() + "\n" +
			"Body: " + request.getBody() ;
		std::cout << "requestDetails: " << requestDetails << std::endl;
		Response* response = requestHandler_.dispatch(request);
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

void Server::handleError(int clientFd, int errorCode) {
	std::cout << "Handling error for client FD: " << clientFd << std::endl;
	std::cout << "serverPort: " << serverConfig_.getPort() << std::endl;
	std::cout << "serverFd: " << serverSocket_.getSocketFd() << std::endl;
	std::cout << "errorCode: " << errorCode << std::endl;

	const std::map<int, std::string>& errorPages = serverConfig_.getErrorPages();
	if (errorPages.find(errorCode) == errorPages.end()) {
		throw std::runtime_error("Error page not found for error code: " + std::to_string(errorCode));
	}

	std::string errorFile = errorPages.at(errorCode);
	std::cout << "Error file (from config): " << errorFile << std::endl;

	// Find the LocationConfig that matches the errorFile path
	const std::vector<LocationConfig>& locations = serverConfig_.getLocations();
	std::string resolvedErrorFile;
	for (size_t i = 0; i < locations.size(); ++i) {
		const LocationConfig& location = locations[i];
		if (errorFile.find(location.getPattern()) == 0) { // Match pattern
			std::string relativePath = errorFile.substr(location.getPattern().size());
			if (!relativePath.empty() && relativePath[0] == '/') {
				relativePath = relativePath.substr(1); // Remove leading slash
			}
			resolvedErrorFile = location.getRoot() + "/" + relativePath;
			break;
		}
	}

	if (resolvedErrorFile.empty()) {
		throw std::runtime_error("No matching location found for error file: " + errorFile);
	}

	std::cout << "Resolved error file path: " << resolvedErrorFile << std::endl;

	// Read the error file
	std::ifstream file(resolvedErrorFile);
	if (!file.is_open()) {
		std::cerr << "Failed to open error file: " << resolvedErrorFile << std::endl;
		closeConnection(clientFd);
		return;
	}

	std::string response((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	// Send the error response
	std::string httpResponse = 
		"HTTP/1.1 " + std::to_string(errorCode) + " Error\r\n" +
		"Content-Length: " + std::to_string(response.size()) + "\r\n" +
		"Connection: close\r\n\r\n" +
		response;

	sendResponse(clientFd, httpResponse);
	closeConnection(clientFd);
}