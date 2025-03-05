#ifndef KQUEUE_HPP
# define KQUEUE_HPP

# include <iostream>
# include <vector>
# include <sys/event.h>
# include <sys/types.h>
# include <unistd.h>

namespace KQUEUE_EVENT {
	enum TYPE {
		SERVER,
		REQUEST,
		RESPONSE
	};
}

typedef struct EventInfo {
	int type;
	int serverFd;

	EventInfo(int type, int serverFd) : type(type), serverFd(serverFd) {}
} EventInfo;

class Kqueue {
private:
	int kqueueFd_;
	int maxEvents_;

	void initialize();
	int getFilter(int eventType);
	Kqueue(void);

public:
	Kqueue(int maxEvents);
	~Kqueue();

	void addEvent(int fd, int eventType, int serverFd);
	void removeEvent(int fd, int filter);
	struct kevent* pollEvents();
};


#endif