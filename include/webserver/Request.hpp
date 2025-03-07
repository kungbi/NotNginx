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
	int fd_;

	bool complete_;
	std::string originalRequest_;

	// 파싱된 내용들 추가
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



	Request();

public:
	Request(int fd);
	// void test();

	bool isComplete() const;
	void appendData(const char* data, size_t length);

	bool equalFd(int fd) const;
	int getFd() const;

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

	void setRequestType(const std::string& type);
	void setTarget(const std::string& target);
	void setPort(const int port);
	void setProtocolVersion(const std::string& version);
	void setHost(const std::string& host);
	void setConnection(const std::string& connection);
	void setContentLength(size_t length);
	void setAccept(const std::string& accept);
	void setQuery(const std::string& query);
	void setFilename(const std::string& filename);
	void setExtension(const std::string& extension);
	void setPath(const std::string& path);
};

#endif