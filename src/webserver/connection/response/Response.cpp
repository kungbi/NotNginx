#include "Response.hpp"

Response::Response(
	const std::string& protocolVersion,
	int statusCode,
	const std::string& reasonPhrase,
	const std::tm& date,
	const std::string& server,
	const std::string& contentType,
	size_t contentLength,
	const std::string& connection,
	const std::string& body
) : protocolVersion_(protocolVersion),
	statusCode_(statusCode),
	reasonPhrase_(reasonPhrase),
	date_(date),
	server_(server),
	contentType_(contentType),
	contentLength_(contentLength),
	connection_(connection),
	body_(body) {}

std::string Response::getResponse(void) {
	std::ostringstream oss;

	// Status Line
	oss << protocolVersion_ << " " << statusCode_ << " " << reasonPhrase_ << "\r\n";

	// Headers
	oss << "Date: " << std::asctime(&date_);
	oss << "Server: " << server_ << "\r\n";
	oss << "Content-Type: " << contentType_ << "\r\n";
	oss << "Content-Length: " << contentLength_ << "\r\n";
	oss << "Connection: " << connection_ << "\r\n";

	// Body
	oss << "\r\n" <<  body_;

	return oss.str();
}

Response::Builder::Builder(void) : protocolVersion_(""), statusCode_(0), reasonPhrase_(""), server_(""), contentType_(""), connection_(""), body_("") {}

Response::Builder& Response::Builder::setProtocolVersion(const std::string& protocolVersion) {
	protocolVersion_ = protocolVersion;
	return *this;
}

Response::Builder& Response::Builder::setStatusCode(int statusCode) {
	statusCode_ = statusCode;
	return *this;
}

Response::Builder& Response::Builder::setReasonPhrase(const std::string& reasonPhrase) {
	reasonPhrase_ = reasonPhrase;
	return *this;
}

Response::Builder& Response::Builder::setServer(const std::string& server) {
	server_ = server;
	return *this;
}

Response::Builder& Response::Builder::setContentType(const std::string& contentType) {
	contentType_ = contentType;
	return *this;
}

Response::Builder& Response::Builder::setConnection(const std::string& connection) {
	connection_ = connection;
	return *this;
}

Response::Builder& Response::Builder::setBody(const std::string& body) {
	body_ = body;
	return *this;
}

void Response::Builder::validate(void) const {
	if (protocolVersion_.empty()) 
		throw std::invalid_argument("Protocol version must not be empty");
	if (statusCode_ == 0) 
		throw std::invalid_argument("Status code must not be 0");
	if (reasonPhrase_.empty())
		throw std::invalid_argument("Reason phrase must not be empty");
	if (server_.empty()) 
		throw std::invalid_argument("Server must not be empty");
	if (contentType_.empty())
		throw std::invalid_argument("Content type must not be empty");
	if (connection_.empty())
		throw std::invalid_argument("Connection must not be empty");
	if (body_.empty())
		throw std::invalid_argument("Body must not be empty");
}

Response Response::Builder::build() const {
	this->validate();

	std::time_t now = std::time(nullptr);
	std::tm& date = *std::gmtime(&now);

	size_t contentLength = body_.size();

	return Response(protocolVersion_, statusCode_, reasonPhrase_, date, server_, contentType_, contentLength, connection_, body_);
}