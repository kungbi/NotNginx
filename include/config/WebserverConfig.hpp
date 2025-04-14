#ifndef WEBSERVERCONFIG_HPP
#define WEBSERVERCONFIG_HPP

#include <vector>
#include <string>
#include <iostream>
#include "HTTPConfig.hpp"
#include "ServerConfig.hpp"

class WebserverConfig {
private:
	HTTPConfig* httpConfig; // HTTP 설정

	WebserverConfig(void);

public:
	// 생성자
	WebserverConfig(HTTPConfig* httpConfig);

	HTTPConfig* getHTTPConfig() const;
};

#endif // WEBSERVERCONFIG_HPP
