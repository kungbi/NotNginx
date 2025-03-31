#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "Kqueue.hpp"
#include "Servers.hpp"
#include "WebserverConfig.hpp"

class Webserver {
public:
	Webserver(Kqueue& kqueue, Servers& servers, WebserverConfig& config);
	void connectClient(struct kevent& event);
	int processClientRequest(struct kevent& event);
	int precessClientResponse(struct kevent& event);
	void processEvents(struct kevent& event);
	void start();
	void sendErrorResponse(int statusCode, const std::string& errorFile);

private:
	Kqueue& kqueue_;
	Servers& servers_;
	WebserverConfig& config_;
};

#endif // WEBSERVER_HPP