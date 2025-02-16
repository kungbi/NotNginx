#ifndef SERVERS_HPP
# define SERVERS_HPP

# include <vector>
# include <memory>
# include <map>
# include <string>
# include "Server.hpp"
# include "Kqueue.hpp"

class Servers {
private:
	std::vector<Server*> servers_; // 서버 리스트 (raw pointer)
	Kqueue& kqueue_;

	Servers(void);

public:
	Servers(Kqueue& kqueue);
	~Servers();

	Server* createServer(Socket& socket, ServerConfig& config, Kqueue& kqueue);
	void addServer(Server &server);
	bool isServerSocketFd(int fd);
	Server* getServerForSocketFd(int fd);
	void handleRequest(int fd);
	size_t size() const;
	Server& getServer(size_t index);

	int connectClient(int serverFd);
	int processRequest(int serverFd, int clientFd);
};


#endif