#ifndef KQUEUE_HPP
# define KQUEUE_HPP

# include <iostream>
# include <vector>
# include <sys/event.h>
# include <sys/types.h>
# include <unistd.h>
# include "Server.hpp"

enum EVENT_TYPE {
	SERVER,
	REQUEST,
	RESPONSE
};

class Kqueue {
private:
	int kqueueFd;
	int maxEvents;

	void initialize();
	int getFilter(int eventType);

public:
	Kqueue(int maxEvents);
	~Kqueue();

	void addEvent(int fd, int eventType, Server& server);
	void removeEvent(int fd, int filter);
	struct kevent* pollEvents();
};


#endif