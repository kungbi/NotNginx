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
public:
    static Request parseRequestHeader(const std::string originalRequest);
    static HeaderValues processHeader(const std::string& header);
    static UriComponents parseUri(const std::string& target);
};

#endif