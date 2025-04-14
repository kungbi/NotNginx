#ifndef HTTPCONFIG_HPP
#define HTTPCONFIG_HPP

#include <string>
#include <vector>
#include "ServerConfig.hpp"
#include <iostream>

class HTTPConfig {
private:
	std::vector<ServerConfig*>* servers; // Store pointers to ServerConfig

public:
	HTTPConfig(std::vector<ServerConfig*>* servers); // Constructor
	~HTTPConfig();

	const std::vector<ServerConfig*>* getServers() const; // Return vector of pointers
};

#endif // HTTPCONFIG_HPP
