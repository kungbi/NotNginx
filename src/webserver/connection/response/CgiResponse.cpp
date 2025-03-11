#include "CgiResponse.hpp"

CgiResponse::CgiResponse(const std::string& response) {
	cgiResponse_ = response;
}

std::string CgiResponse::getResponse(void) {
	return cgiResponse_;
}
