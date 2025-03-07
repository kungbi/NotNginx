#include "Request.hpp"


Request::Request(
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
	const std::string& body
) : requestType_(requestType),
	protocolVersion_(protocolVersion),
	host_(host),
	target_(target),
	query_(query),
	fileName_(fileName),
	extension_(extension),
	path_(path),
	port_(port),
	connection_(connection),
	contentLength_(contentLength),
	accept_(accept),
	body_(body) {}

RequestType Request::getRequestType() const {
	return requestType_;
}

std::string Request::getProtocolVersion() const {
	return protocolVersion_;
}

std::string Request::getHost() const {
	return host_;
}

int Request::getPort() const {
	return port_;
}

std::string Request::getConnection() const {
	return connection_;
}

size_t Request::getContentLength() const {
	return contentLength_;
}

std::string Request::getAccept() const {
	return accept_;
}

std::string Request::getTarget() const {
	return target_;
}

std::string Request::getQuery() const {
	return query_;
}

std::string Request::getFilename() const {
	return fileName_;
}

std::string Request::getExtension() const {
	return extension_;
}

std::string Request::getPath() const {
	return path_;
}

std::string Request::getBody() const {
	return body_;
}