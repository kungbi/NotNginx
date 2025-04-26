#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include <iostream>
# include <string>
# include <queue>
# include "Responses.hpp"

class Connection {
private:
	int fd_;
	std::string originalRequest_;
	Responses responses_;

	Connection(void);

public:
	Connection(int fd);
	~Connection(void);

	// request handling
	size_t getOriginalRequestSize(void) const;
	size_t getBodySize(void) const;
	void appendRequestData(const char* data, size_t length);
	bool hasRequest(void) const;
	std::string getRequest(void);

	// response handling
	void addResponse(Response& response);
	bool hasResponse(void) const;
	Response* getResponse(void);
	void appendCgiBuffer(int pipeFd, const std::string& data, bool isEnd);
};

#endif