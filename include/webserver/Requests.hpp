#ifndef REQUESTS_HPP
# define REQUESTS_HPP

# include <iostream>
# include "Request.hpp"
# include <map>

class Requests {
private:
	std::map<int, Request*> requests_;

public:
	Requests();

	void addRequest(Request* request);
	size_t size() const;
	bool isExist(int fd) const;
	Request* getRequest(int fd);
	void removeRequest(int fd);
};

#endif