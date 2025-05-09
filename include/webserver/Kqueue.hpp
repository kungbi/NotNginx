#ifndef KQUEUE_HPP
# define KQUEUE_HPP

# include <iostream>
# include <vector>
# include <sys/event.h>
# include <sys/types.h>
# include <unistd.h>
# include "HttpExceptions.hpp"
# include <map>

namespace KQUEUE_EVENT {
	enum TYPE {
		SERVER,
		REQUEST,
		RESPONSE,
		CGI_RESPONSE
	};
}

typedef struct EventInfo {
	int type;
	int serverFd;
	int clientFd;

	EventInfo(int type, int serverFd) : type(type), serverFd(serverFd) {}
	EventInfo(int type, int serverFd, int clientFd) : type(type), serverFd(serverFd), clientFd(clientFd) {}
} EventInfo;

const int MAX_EVENTS = 1024;
class Kqueue {
private:
	int kqueueFd_;
	std::map<std::pair<int, int>, EventInfo*> eventInfoMap_;

	void initialize();
	int getFilter(int eventType);

public:
	Kqueue(void);
	~Kqueue(void);

	void addEvent(int fd, int eventType, int serverFd);
	void addEvent(int fd, int eventType, int clientFd, int serverFd);
	void removeEvent(int fd, int filter);
	struct kevent* pollEvents();
};


#endif