#include "CgiResponse.hpp"

CgiResponse::~CgiResponse(void) {}

CgiResponse::CgiResponse(const std::string& response) {
	cgiResponse_ = response;
}


std::string CgiResponse::getResponse(void) {
	std::ostringstream oss;

	// Status Line
	oss << "HTTP/1.1 200" << "\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << cgiResponse_.size() << "\r\n";
	oss << "Connection: close\r\n";
	oss << "\r\n";

	// Body
	oss << "\r\n" <<  cgiResponse_;

	return oss.str();
}
