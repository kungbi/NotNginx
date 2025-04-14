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

	void processEvents(struct kevent& event);
	void connectClient(struct kevent& event);
	int processClientRequest(struct kevent& event);
	int precessClientResponse(struct kevent& event);
	int processCgiResponse(struct kevent& event);

	Webserver(void);

public:
	Webserver(Kqueue& kqueue, Servers& servers, WebserverConfig& config);
	void start();
};


#endif