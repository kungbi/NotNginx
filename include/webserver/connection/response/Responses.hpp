#ifndef RESPONSES_HPP
# define RESPONSES_HPP

# include "Response.hpp"
# include "CgiResponse.hpp"

class Responses {
private:
	std::queue<Response*> responses_;

public:
	Responses();
	~Responses();

	void addResponse(Response& response);
	bool hasResponse() const;
	Response* popResponse();
};

#endif