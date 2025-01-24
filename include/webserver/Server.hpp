#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <map>
# include "Socket.hpp"
# include "ServerConfig.hpp"
# include "Kqueue.hpp"

class Server {
private:
	Socket& serverSocket;
	ServerConfig& serverConfig;
	Kqueue& kqueue;

public:
	Server(Socket &serverSocket, ServerConfig& serverConfig, Kqueue& kqueue);
	int getSocketFd() const;
	int acceptClient();
	int handleRequest(int clientFd);
};


#endif