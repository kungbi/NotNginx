#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <map>
# include "Socket.hpp"
# include "ServerConfig.hpp"
# include "Kqueue.hpp"
# include "Connections.hpp"
# include "Request.hpp"
# include "RequestParser.hpp"
# include "Response.hpp"
# include "StaticResourceResponse.hpp"
# include "Router.hpp"
# include "CgiHandler.hpp"
# include "StaticResourceHandler.hpp"
# include "RequestHandler.hpp"

class Server {
private:
	Socket& serverSocket_;
	ServerConfig& serverConfig_;
	Kqueue& kqueue_;
	Connections connections_;
	RequestHandler& requestHandler_;
	
	Server(void);
	int processClientData(int clientFd, const char* buffer, ssize_t bytesRead);
	void sendResponse(int clientFd, const std::string& response);
	std::string resolveErrorFile(int errorCode);
	std::string readErrorFile(const std::string& filePath);
	void sendErrorResponse(int clientFd, int errorCode, const std::string& response);

public:
	Server(Socket& serverSocket, ServerConfig& serverConfig, Kqueue& kqueue, RequestHandler& requestHandler);
	int getSocketFd() const;
	int acceptClient();
	int handleRequest(int clientFd);
	int handleResponse(int clientFd);
	void closeConnection(int clientFd);
	void handleError(int clientFd, int errorCode);
};

#endif
