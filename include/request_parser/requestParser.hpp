#ifndef REQUEST_PARSER_HPP
# define REQUEST_PARSER_HPP

# include <sstream>

class Request;  // forward declaration

struct UriComponents {
	std::string path;
	std::string query;
	std::string filename;
	std::string extension;
};

struct HeaderValues {
	std::string host;
	int port = 0;
	std::string connection;
	size_t contentLength = 0;
	std::string accept;
	std::string contentType;
};

class RequestParser {
private:
	static std::tuple<std::string, std::istringstream> parseRequestLine(std::istringstream& stream);
	static std::tuple<std::string, int, std::string, size_t, std::string, std::string> parseHeaders(std::istringstream& stream);
	static std::string parseBody(std::istringstream& stream, size_t contentLength);
	
public:
	static Request parseRequestHeader(const std::string originalRequest);
	static HeaderValues processHeader(const std::string& header);
	static UriComponents parseUri(const std::string& target);

};

#endif