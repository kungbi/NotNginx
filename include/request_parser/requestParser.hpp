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
        void parseRequestHeader(Request* request);
        static UriComponents parseUri(const std::string& target);
        void processHeader(Request* request, const std::string& header);
};

#endif