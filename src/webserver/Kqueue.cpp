#include "Kqueue.hpp"

void Kqueue::initialize() {
	kqueueFd_ = kqueue();
	if (kqueueFd_ == -1) {
		throw std::runtime_error("Failed to initialize kqueue");
	}
	
	std::cout << "Kqueue initialized with FD: " << kqueueFd_ << std::endl;
}

Kqueue::Kqueue(int maxEvents) : maxEvents_(maxEvents) {
	initialize();
}

Kqueue::~Kqueue() {
	if (close(kqueueFd_) == -1) {
		throw std::runtime_error("Failed to close kqueue");
	}

	std::cout << "Kqueue closed." << std::endl;
}

int Kqueue::getFilter(int eventType) {
	if (eventType == KQUEUE_EVENT::SERVER) {
		return EVFILT_READ;
	}
	if (eventType == KQUEUE_EVENT::REQUEST) {
		return EVFILT_READ;
	}
	if (eventType == KQUEUE_EVENT::RESPONSE) {
		return EVFILT_WRITE;
	}
	throw std::runtime_error("Invalid event type");
}

void Kqueue::addEvent(int fd, int eventType, int serverFd) {
	int filter = getFilter(eventType);
	struct kevent event;
	EventInfo* eventInfo = new EventInfo(eventType, serverFd);

	EV_SET(&event, fd, filter, EV_ADD | EV_ENABLE, 0, 0, eventInfo);
	if (kevent(kqueueFd_, &event, 1, nullptr, 0, nullptr) == -1) {
		throw std::runtime_error("Failed to add FD to kqueue");
	}

	std::cout << "FD: " << fd << " added with filter: " << filter << std::endl;
}

void Kqueue::removeEvent(int fd, int filter) {
	struct kevent event;
	EV_SET(&event, fd, filter, EV_DELETE, 0, 0, nullptr);
	if (kevent(kqueueFd_, &event, 1, nullptr, 0, nullptr) == -1) {
		throw std::runtime_error("Failed to remove FD from kqueue");
	}

	std::cout << "FD: " << fd << " removed." << std::endl;
}

static const int TIMEOUT_MS = -1;

struct kevent* Kqueue::pollEvents() {
	struct kevent events[maxEvents_];
	struct timespec timeout;
	struct timespec *timeoutPtr = nullptr;

	if (TIMEOUT_MS >= 0) {
		timeout.tv_sec = TIMEOUT_MS / 1000;
		timeout.tv_nsec = (TIMEOUT_MS % 1000) * 1000000;
		timeoutPtr = &timeout;
	}

	int eventCount = kevent(kqueueFd_, nullptr, 0, events, maxEvents_, timeoutPtr);
	if (eventCount == -1) {
		perror("Error polling kqueue events");
		throw std::runtime_error("Error polling kqueue events");
	}
	if (eventCount == 0) {
		std::cout << "No events to process." << std::endl;
		return nullptr;
	}

	struct kevent* returnEvent = new struct kevent[eventCount];
	std::copy(events, events + eventCount, returnEvent);
	return returnEvent;
}
