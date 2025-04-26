#ifndef CONNECTIONS_HPP
# define CONNECTIONS_HPP

# include "Connection.hpp"
# include <map>
# include <string>

class Connections {
private:
	std::map<int, Connection*> connections_;

	const Connection& getConnection(int fd) const;

public:
	Connections(void);
	~Connections(void);

	// Connection handling
	void addConnection(int fd);
	void removeConnection(int fd);
	bool hasConnection(int fd) const;
	
	// Request handling
	size_t getOriginalRequestSize(int fd) const;
	size_t getBodySize(int fd) const;
	bool hasRequest(int fd) const;
	void appendRequestData(int fd, const char* data, size_t length);
	std::string getRequest(int fd);

	// Response handling
	bool hasResponse(int fd) const;
	void addResponse(int fd, Response& response);
	Response* getResponse(int fd);

	void appendCgiBuffer(int fd, int pipeFd, const std::string& data, bool isEnd);
};

#endif