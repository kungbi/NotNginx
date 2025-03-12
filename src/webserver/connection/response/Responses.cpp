#include "Responses.hpp"

Responses::Responses() {
}

Responses::~Responses() {
	while (!responses_.empty()) {
		delete responses_.front();
		responses_.pop();
	}
}

void Responses::addResponse(Response& response) {
	responses_.push(&response);
}

bool Responses::hasResponse() const {
	return !responses_.empty();
}

Response* Responses::popResponse() {
	Response* response = responses_.front();
	responses_.pop();
	return response;
}

void Responses::appendCgiBuffer(int fd, const std::string& data, bool isEnd) {
	cgiBuffer_.appendBuffer(fd, data);

	if (isEnd) {
		std::string buffer = cgiBuffer_.popBuffer(fd);
		if (!buffer.empty()) {
			Response* response = new CgiResponse(buffer);
			this->addResponse(*response);
		}
	}
}