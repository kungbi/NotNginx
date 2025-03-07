#ifndef REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP

# include <sstream>
# include <string>

class Request;  // forward declaration

struct UriComponents {
	std::string path;
	std::string query;
	std::string filename;
	std::string extension;
};

class RequestParser {
public:
	Request parseRequestHeader(const std::string originalRequest);
	static UriComponents parseUri(const std::string& target);
	void processHeader(const std::string& header, std::string& host, int& port, std::string& connection, size_t& contentLength, std::string& accept);
};

#endif