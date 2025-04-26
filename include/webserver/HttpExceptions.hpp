#ifndef HTTP_EXCEPTIONS_HPP
#define HTTP_EXCEPTIONS_HPP

#include <stdexcept>
#include <string>

// Base class for all HTTP-related exceptions
class HttpException : public std::runtime_error {
public:
	explicit HttpException(const std::string& message)
		: std::runtime_error(message) {}

	// Virtual method to get the HTTP status code
	virtual int getStatusCode() const = 0;
};

// Derived exception for 404 Not Found
class NotFoundError : public HttpException {
public:
	explicit NotFoundError(const std::string& message)
		: HttpException(message) {}

	int getStatusCode() const {
		return 404;
	}
};

// Derived exception for 400 Bad Request
class BadRequestError : public HttpException {
public:
	explicit BadRequestError(const std::string& message)
		: HttpException(message) {}

	int getStatusCode() const {
		return 400;
	}
};

// Derived exception for 403 Forbidden
class ForbiddenError : public HttpException {
public:
	explicit ForbiddenError(const std::string& message)
		: HttpException(message) {}

	int getStatusCode() const {
		return 403;
	}
};

// Derived exception for 500 Internal Server Error
class InternalServerError : public HttpException {
public:
	explicit InternalServerError(const std::string& message)
		: HttpException(message) {}

	int getStatusCode() const {
		return 500;
	}
};

// Derived exception for 401 Unauthorized
class UnauthorizedError : public HttpException {
public:
	explicit UnauthorizedError(const std::string& message)
		: HttpException(message) {}

	int getStatusCode() const {
		return 401;
	}
};

// Derived exception for 405 Method Not Allowed
class MethodNotAllowedError : public HttpException {
public:
	explicit MethodNotAllowedError(const std::string& message)
		: HttpException(message) {}

	int getStatusCode() const {
		return 405;
	}
};

class RequestEntityTooLargeError : public HttpException {
public:
	explicit RequestEntityTooLargeError(const std::string& message)
		: HttpException(message) {}

	int getStatusCode() const {
		return 413;
	}
};

// Add more custom exceptions as needed, inheriting from HttpException

#endif // HTTP_EXCEPTIONS_HPP
