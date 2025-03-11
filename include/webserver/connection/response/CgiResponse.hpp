#ifndef CGI_RESPONSE_HPP
# define CGI_RESPONSE_HPP

# include "Response.hpp"
# include <string>

class CgiResponse: public Response {
private:
	std::string cgiResponse_;

public:
	CgiResponse(const std::string& response);

	std::string getResponse();
};

#endif