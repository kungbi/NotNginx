#ifndef STATIC_RESOURCE_RESPONSE_HPP
# define STATIC_RESOURCE_RESPONSE_HPP

# include <string>
# include <ctime>
# include <sstream>
# include "Response.hpp"

class StaticResourceResponse: public Response {
private:
	// Status Line
	std::string protocolVersion_;
	int statusCode_;
	std::string reasonPhrase_;

	// Headers
	std::tm date_;
	std::string server_;
	std::string contentType_;
	size_t contentLength_;
	std::string connection_;

	// Body
	std::string body_;

	StaticResourceResponse(void);
	StaticResourceResponse(
		const std::string& protocolVersion,
		int statusCode,
		const std::string& reasonPhrase,
		const std::tm& date,
		const std::string& server,
		const std::string& contentType,
		size_t contentLength,
		const std::string& connection,
		const std::string& body
	);

public:
	class Builder;

	std::string getResponse(void);
};

class StaticResourceResponse::Builder {
private:
	std::string protocolVersion_;
	int statusCode_;
	std::string reasonPhrase_;
	std::string server_;
	std::string contentType_;
	std::string connection_;
	std::string body_;

	void validate(void) const;

public:
	Builder(void);

	Builder& setProtocolVersion(const std::string& protocolVersion);
	Builder& setStatusCode(int statusCode);
	Builder& setReasonPhrase(const std::string& reasonPhrase);
	Builder& setServer(const std::string& server);
	Builder& setContentType(const std::string& contentType);
	Builder& setConnection(const std::string& connection);
	Builder& setBody(const std::string& body);

	StaticResourceResponse build() const;
};

#endif // STATIC_RESOURCE_RESPONSE_HPP