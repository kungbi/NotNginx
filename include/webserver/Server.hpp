#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <map>
# include "Socket.hpp"
# include "ServerConfig.hpp"
# include "Kqueue.hpp"
# include "Connections.hpp"

class Server {
private:
	Socket& serverSocket_;
	ServerConfig& serverConfig_;
	Kqueue& kqueue_;
	Connections connections_;


	Server(void);

	int processClientData(int clientFd, const char* buffer, ssize_t bytesRead);
	void sendResponse(int clientFd, const std::string& response);
	void addRequestBuffer(int clientFd, const char* buffer, size_t bytesRead) const;

public:
	Server(Socket &serverSocket, ServerConfig& serverConfig, Kqueue& kqueue);
	int getSocketFd() const;
	int acceptClient();
	int handleRequest(int clientFd);
};


#endif