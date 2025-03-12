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
	: serverSocket_(serverSocket), serverConfig_(serverConfig), kqueue_(kqueue) {
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
	std::cout << "Bytes read: " << bytesRead << std::endl;
	
	if (!this->connections_.hasConnection(clientFd)) {
		this->connections_.addConnection(clientFd);
	}

	this->connections_.appendRequestData(clientFd, buffer, bytesRead);

	if (this->connections_.hasRequest(clientFd)) {
		Request request = RequestParser::parseRequestHeader(this->connections_.getRequest(clientFd));

		// 요청 처리 로직
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

		StaticResourceResponse response = StaticResourceResponse::Builder()
			.setProtocolVersion("HTTP/1.1")
			.setStatusCode(200)
			.setReasonPhrase("OK")
			.setServer("Server")
			.setContentType("text/html")
			.setConnection("close")
			.setBody(
				"<html>\n" 
				"<body>\n" 
					"<h1>Welcome to our website</h1>\n"
					"<pre>" + requestDetails + "</pre>\n"
				"</body>\n" 
				"</html>"
			)
			.build();
		
		sendResponse(clientFd, response.getResponse());
		return 0;
	}

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
	std::cout << "Bytes read: " << bytesRead << std::endl;

	if (bytesRead > 0) {
		buffer[bytesRead] = '\0'; // Null-terminate for safety
		return processClientData(clientFd, buffer, bytesRead);
	}
	
	if (bytesRead == 0) {
		// 클라이언트가 연결을 닫은 경우
		std::cout << "Client disconnected on FD: " << clientFd << std::endl;
		// kqueue.removeEvent(fd, EVFILT_READ); // Kqueue에서 제거
		close(clientFd); // 소켓 닫기
		return 1;
	}
	
	perror("Error reading from FD");
	// kqueue.removeEvent(clientFd, EVFILT_READ); // Kqueue에서 제거
	close(clientFd); // 소켓 닫기
	return 1;
}

void Server::closeConnection(int clientFd) {
	close(clientFd);
	this->connections_.removeConnection(clientFd);
}