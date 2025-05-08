#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>
# include "HttpExceptions.hpp"

enum RequestType {
	GET,
	POST,
	PUT,
	PATCH,
	DELETE,
};

class Request {
private:
	RequestType requestType_;
	std::string protocolVersion_;
	std::string host_;
	std::string target_;
	std::string query_;
	std::string fileName_;
	std::string extension_;
	std::string path_; 

	int port_;
	std::string connection_;
	size_t contentLength_;
	std::string accept_;
	std::string body_;
	std::string contentType_; // Add this member variable

	Request();

public:
	Request(
		RequestType requestType,
		const std::string& protocolVersion,
		const std::string& host,
		const std::string& target,
		const std::string& query,
		const std::string& fileName,
		const std::string& extension,
		const std::string& path,
		int port,
		const std::string& connection,
		size_t contentLength,
		const std::string& accept,
		const std::string& body,
		const std::string& contentType
	);

	const std::string& getOriginalRequest() const;
	RequestType getRequestType() const;
	std::string getProtocolVersion() const;
	std::string getHost() const;
	std::string getTarget() const;
	int getPort() const;
	std::string getConnection() const;
	size_t getContentLength() const;
	std::string getAccept() const;
	std::string getQuery() const;
	std::string getFilename() const;
	std::string getExtension() const;
	std::string getPath() const;
	std::string getBody() const;
	std::string getContentType() const; 
};

#endif