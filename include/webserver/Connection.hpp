#ifndef CONNECTION_HPP
# define CONNECTION_HPP

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

	// request handling
	void appendRequestData(const char* data, size_t length);
	bool hasRequest(void) const;
	std::string getRequest(void);

	// response handling
	void addResponse(const Response& response);
	bool hasResponse(void) const;
	Response getResponse(void);
};

#endif