#ifndef RESPONSES_HPP
# define RESPONSES_HPP

# include "Response.hpp"
# include "CgiResponse.hpp"
# include "CgiBuffer.hpp"
# include <queue>

class Responses {
private:
	std::queue<Response*> responses_;
	CgiBuffer cgiBuffer_;

public:
	Responses();
	~Responses();

	void addResponse(Response& response);
	bool hasResponse() const;
	Response* popResponse();

	void appendCgiBuffer(int fd, const std::string& data, bool isEnd);
};

#endif