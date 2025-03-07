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

class RequestParser {
public:
    static Request parseRequestHeader(const std::string originalRequest);
    static void processHeader(const std::string& header, std::string& host, int& port, std::string& connection, size_t& contentLength, std::string& accept);
    static UriComponents parseUri(const std::string& target);
};

#endif