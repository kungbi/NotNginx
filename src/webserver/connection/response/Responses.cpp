#include "Responses.hpp"

Responses::Responses() {
}

Responses::~Responses() {
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
