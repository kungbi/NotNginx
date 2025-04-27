#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include <unordered_map>
# include <vector>
# include <string>
# include <iostream>
# include "Kqueue.hpp"
# include "Servers.hpp"
# include "WebserverConfig.hpp"

class Webserver {
private:
	Kqueue& kqueue_;
	Servers& servers_;
	WebserverConfig& config_;

	void processEvents(int fd, EventInfo* eventInfo);
	void connectClient(EventInfo* eventInfo);
	int processClientRequest(int fd, EventInfo* eventInfo);
	int processClientResponse(int fd, EventInfo* eventInfo);
	int processCgiResponse(int fd, EventInfo *eventInfo);

	Webserver(void);

public:
	Webserver(Kqueue& kqueue, Servers& servers, WebserverConfig& config);
	void start();
};


#endif