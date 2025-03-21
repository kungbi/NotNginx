#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include "LocationConfig.hpp"

class ServerConfig {
private:
	std::string host;
	int port;
	std::string serverName;
	size_t clientMaxBodySize;
	std::map<int, std::string> errorPages;
	std::vector<LocationConfig> locations;
	bool isDefault;

	ServerConfig(void);

public:
	ServerConfig(
		const std::string host,
		const int port,
		const std::string serverName,
		const size_t clientMaxBodySize,
		const std::map<int, std::string>& errorPages,
		const std::vector<LocationConfig>& locations,
		const bool isDefault
	);

	std::string getHost() const;
	int getPort() const;
	std::string getServerName() const;
	size_t getClientMaxBodySize() const;
	const std::vector<LocationConfig>& getLocations() const;
};

#endif
