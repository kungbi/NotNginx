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
	while (true) {
		struct kevent* event = kqueue_.pollEvents();
		int fd = event->ident;
		EventInfo* eventInfo = (EventInfo *) event->udata;

		try {
			processEvents(*event);
		} catch (const HttpException& e) {
			Server* server = servers_.getServerForSocketFd(eventInfo->serverFd);

				server->handleError(fd, e.getStatusCode());
		}

		delete[] event;
	}
}
