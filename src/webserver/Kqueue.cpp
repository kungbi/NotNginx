#include "Kqueue.hpp"

void Kqueue::initialize() {
	kqueueFd_ = kqueue();
	if (kqueueFd_ == -1) {
		throw std::runtime_error("Failed to initialize kqueue");
	}
	
	std::cout << "Kqueue initialized with FD: " << kqueueFd_ << std::endl;
}

Kqueue::Kqueue() {
	initialize();
}

Kqueue::~Kqueue() {
	for (std::map<std::pair<int, int>, EventInfo*>::iterator it = eventInfoMap_.begin(); it != eventInfoMap_.end(); ++it) {
		delete it->second;
	}
	eventInfoMap_.clear();

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
	if (eventType == KQUEUE_EVENT::CGI_RESPONSE) {
		return EVFILT_READ;
	}
	throw std::runtime_error("Invalid event type");
}

void Kqueue::addEvent(int fd, int eventType, int serverFd) {
	int filter = getFilter(eventType);
	struct kevent event;
	EventInfo* eventInfo = new EventInfo(eventType, serverFd);

	if (eventInfoMap_[std::make_pair(fd, eventType)]) {
		delete eventInfoMap_[std::make_pair(fd, eventType)];
	}
	eventInfoMap_[std::make_pair(fd, eventType)] = eventInfo;

	EV_SET(&event, fd, filter, EV_ADD | EV_ENABLE, 0, 0, eventInfo);
	if (kevent(kqueueFd_, &event, 1, nullptr, 0, nullptr) == -1) {
		throw std::runtime_error("Failed to add FD to kqueue");
	}

	std::cout << "FD: " << fd << " added with filter: " << filter << std::endl;
}

void Kqueue::addEvent(int fd, int eventType, int clientFd, int serverFd) {
	int filter = getFilter(eventType);
	struct kevent event;
	EventInfo* eventInfo = new EventInfo(eventType, serverFd, clientFd);

	eventInfoMap_[std::make_pair(fd, eventType)] = eventInfo;

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
		std::cerr << "kevent EV_DELETE failed for fd=" << fd << ": " << strerror(errno) << std::endl;
		throw std::runtime_error("Failed to remove FD from kqueue");
	}

	std::pair<int, int> key = std::make_pair(fd, filter);
	std::map<std::pair<int, int>, EventInfo*>::iterator it = eventInfoMap_.find(key);
	if (it != eventInfoMap_.end()) {
		delete it->second;
		eventInfoMap_.erase(it);
	}

	std::cout << "FD: " << fd << " removed." << std::endl;
}

static const int TIMEOUT_MS = 3000;

struct kevent* Kqueue::pollEvents() {
	struct kevent events[MAX_EVENTS];
	struct timespec timeout;
	struct timespec *timeoutPtr = nullptr;

	if (TIMEOUT_MS >= 0) {
		timeout.tv_sec = TIMEOUT_MS / 1000;
		timeout.tv_nsec = (TIMEOUT_MS % 1000) * 1000000;
		timeoutPtr = &timeout;
	}

	int eventCount = kevent(kqueueFd_, nullptr, 0, events, MAX_EVENTS, timeoutPtr);
	if (eventCount == -1) {
		if (errno == EINTR) {
			return pollEvents();
		}
		throw std::runtime_error("Error polling kqueue events");
	}
	if (eventCount == 0) {
		return nullptr;
	}

	struct kevent* returnEvent = new struct kevent[eventCount];
	std::copy(events, events + eventCount, returnEvent);
	return returnEvent;
}
