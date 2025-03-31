#include "Webserver.hpp"
#include <fstream>
#include "HttpExceptions.hpp" // Include the custom exceptions header

Webserver::Webserver(Kqueue& kqueue, Servers& servers, WebserverConfig& config)
	: kqueue_(kqueue), servers_(servers), config_(config) {}
	
void Webserver::connectClient(struct kevent& event) {
	int serverFd = ((EventInfo *) event.udata)->serverFd;
	int clientFd = servers_.connectClient(serverFd);

	if (clientFd == -1)  {
		throw std::runtime_error("Failed to accept client");
	}

	kqueue_.addEvent(clientFd, KQUEUE_EVENT::REQUEST, serverFd);
}

int Webserver::processClientRequest(struct kevent& event) {
	int serverFd = ((EventInfo *) event.udata)->serverFd;
	int clientFd = event.ident;

	return servers_.processRequest(serverFd, clientFd);
}

int Webserver::precessClientResponse(struct kevent& event) {
	int serverFd = ((EventInfo *) event.udata)->serverFd;
	int clientFd = event.ident;

	return servers_.processResponse(serverFd, clientFd);
}

void Webserver::processEvents(struct kevent& event) {
	int fd = event.ident;
	EventInfo* eventInfo = (EventInfo *) event.udata;

	std::cout << "Processing event for FD: " << fd << std::endl;

	if (eventInfo->type == KQUEUE_EVENT::SERVER) {
		std::cout << "Server event." << std::endl;
		connectClient(event);
	}

	if (eventInfo->type == KQUEUE_EVENT::REQUEST) {
		std::cout << "Request event." << std::endl;
		if (processClientRequest(event) == 0) {
			delete eventInfo;
		}
	}

	if (eventInfo->type == KQUEUE_EVENT::RESPONSE) {
		std::cout << "Response event." << std::endl;
		if (precessClientResponse(event) == 0) {
			delete eventInfo;
		}
	}
}

void Webserver::start() {
	std::cout << "Webserver started." << std::endl;

	while (true) {
		struct kevent* event = kqueue_.pollEvents();
		int fd = event->ident;
		EventInfo* eventInfo = (EventInfo *) event->udata;

		std::cout << "Event identified for FD: " << fd << std::endl;
		std::cout << "Event type: " << eventInfo->type << std::endl;

		try {
			processEvents(*event);
		} catch (const HttpException& e) {
			std::cerr << "HttpException: " << e.what() << std::endl;
			Server* server = servers_.getServerForSocketFd(eventInfo->serverFd);
			server->handleError(fd, e.getStatusCode());
		}

		delete[] event;
	}
}

void Webserver::sendErrorResponse(int statusCode, const std::string& errorFile) {
	std::ifstream file(errorFile);
	if (!file.is_open()) {
		std::cerr << "Failed to open error file: " << errorFile << std::endl;
		return;
	}

	std::string response((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	// Logic to send the response to the client
	std::cout << "Sending error response (" << statusCode << "): " << errorFile << std::endl;
	// ...send response to client...
}
