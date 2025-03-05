#include "Webserver.hpp"

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
	}

}

void Webserver::start() {
	std::cout << "Webserver started." << std::endl << std::endl;

	while (true) {
		struct kevent* event = kqueue_.pollEvents();
		processEvents(*event); // 이벤트 처리
		std::cout << "================" << std::endl;

		delete[] event; // 메모리 해제
	}
}
