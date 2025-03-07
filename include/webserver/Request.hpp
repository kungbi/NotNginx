#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>

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

	Request(void);

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
		const std::string& accept
	);
};

#endif