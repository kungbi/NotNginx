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

	if (eventInfo->type == KQUEUE_EVENT::CGI_RESPONSE) {
		std::cout << "CGI response event." << std::endl;
		char buffer[1024];
		ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
		std::cout << "Read bytes: " << bytesRead << std::endl;
		if (bytesRead == -1 && errno == EAGAIN) {
			// 아직 데이터가 안 온 것일 수 있음 (non-blocking일 경우)
			return;
		}
		if (bytesRead <= 0) {
			std::cout << "CGI pipe closed or failed. bytesRead: " << bytesRead << std::endl;

			kqueue_.removeEvent(fd, KQUEUE_EVENT::CGI_RESPONSE);
			close(fd);
			delete eventInfo;
			return;
		}
		std::cout << "CGI response data: " << buffer << std::endl;
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
			std::cerr << "HTTP Exception: " << e.what() << std::endl;
			std::cerr << "Status Code: " << e.getStatusCode() << std::endl;
			Server* server = servers_.getServerForSocketFd(eventInfo->serverFd);
			server->handleError(fd, e.getStatusCode());
		}

		delete[] event;
	}
}
