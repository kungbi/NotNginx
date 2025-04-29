#include "ErrorPageGenerator.hpp"

std::string generateDefaultErrorPage(int errorCode) {
	std::string reason;

	switch (errorCode) {
		case 400: reason = "Bad Request"; break;
		case 401: reason = "Unauthorized"; break;
		case 403: reason = "Forbidden"; break;
		case 404: reason = "Not Found"; break;
		case 405: reason = "Method Not Allowed"; break;
		case 413: reason = "Payload Too Large"; break;
		case 500: reason = "Internal Server Error"; break;
		case 502: reason = "Bad Gateway"; break;
		case 503: reason = "Service Unavailable"; break;
		default:  reason = "Error"; break;
	}

	std::string body =
		"<html>\n"
		"<head><title>" + std::to_string(errorCode) + " " + reason + "</title></head>\n"
		"<body>\n"
		"<h1>" + std::to_string(errorCode) + " " + reason + "</h1>\n"
		"<p>The server encountered an error and could not complete your request.</p>\n"
		"</body>\n"
		"</html>\n";

	return body;
}
