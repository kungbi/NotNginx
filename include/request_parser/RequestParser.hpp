#ifndef REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP

# include <sstream>
# include <tuple>
# include <string>
# include "Request.hpp"

class Request; 


struct HeaderResult {
	std::string host;
	int port;
	std::string connection;
	size_t contentLength;
	std::string accept;
	std::string contentType;
};

struct UriComponents {
	std::string path;
	std::string query;
	std::string filename;
	std::string extension;
};

struct HeaderValues {
	std::string host;
	int port;
	std::string connection;
	size_t contentLength;
	std::string accept;
	std::string contentType;
};


class RequestParser {
private:
	static HeaderResult parseHeaders(std::istringstream& stream);
	static std::string parseBody(std::istringstream& stream, size_t contentLength);
	
public:
	static Request parseRequestHeader(const std::string& originalRequest);
	static UriComponents parseUri(const std::string& target);

};

#endif