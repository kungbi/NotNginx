#include "Webserver.hpp"
#include <fstream>
#include "HttpExceptions.hpp" // Include the custom exceptions header

Webserver::Webserver(Kqueue& kqueue, Servers& servers, WebserverConfig& config)
	: kqueue_(kqueue), servers_(servers), config_(config) {}
	
void Webserver::connectClient(EventInfo *eventInfo) {
	int serverFd = eventInfo->serverFd;
	int clientFd = servers_.connectClient(serverFd);

	if (clientFd == -1)  {
		throw std::runtime_error("Failed to accept client");
	}

	kqueue_.addEvent(clientFd, KQUEUE_EVENT::REQUEST, serverFd);
}

int Webserver::processClientRequest(int fd, EventInfo *eventInfo) {
	int serverFd = eventInfo->serverFd;
	int clientFd = fd;

	return servers_.processRequest(serverFd, clientFd);
}

int Webserver::processClientResponse(int fd, EventInfo *eventInfo) {
	int serverFd = eventInfo->serverFd;
	int clientFd = fd;

	return servers_.processResponse(serverFd, clientFd);
}

int Webserver::processCgiResponse(int fd, EventInfo *eventInfo) {
	int serverFd = eventInfo->serverFd;
	int clientFd = eventInfo->clientFd;

	return this->servers_.processCgiResponse(serverFd, clientFd, fd);
}

void Webserver::processEvents(int fd, EventInfo* eventInfo) {
	std::cout << "Processing event for FD: " << fd << std::endl;

	if (eventInfo->type == KQUEUE_EVENT::SERVER) {
		std::cout << "Server event." << std::endl;
		connectClient(eventInfo);
	}

	if (eventInfo->type == KQUEUE_EVENT::REQUEST) {
		std::cout << "Request event." << std::endl;
		if (processClientRequest(fd, eventInfo) == 0) {
			delete eventInfo;
		}
	}

	if (eventInfo->type == KQUEUE_EVENT::RESPONSE) {
		std::cout << "Response event." << std::endl;
		if (processClientResponse(fd, eventInfo) == 0) {
			delete eventInfo;
		}
	}

	if (eventInfo->type == KQUEUE_EVENT::CGI_RESPONSE) {
        std::cout << "CGI response event." << std::endl;
        if (processCgiResponse(fd, eventInfo) == 0) {
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
			processEvents(fd, eventInfo);
		} catch (const HttpException& e) {
			std::cerr << "HTTP Exception: " << e.what() << std::endl;
			std::cerr << "Status Code: " << e.getStatusCode() << std::endl;
			Server* server = servers_.getServerForSocketFd(eventInfo->serverFd);
			server->handleError(fd, e.getStatusCode());
		}

		delete[] event;
	}
}
