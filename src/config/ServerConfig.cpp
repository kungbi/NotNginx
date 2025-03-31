#include "ServerConfig.hpp"
#include <iostream>

ServerConfig::ServerConfig(
	const std::string host,
	const int port,
	const std::string serverName,
	const size_t clientMaxBodySize, // (mb)
	const std::map<int, std::string>& errorPages,
	const std::vector<LocationConfig>& locations,
	const bool defaultServer // Updated parameter name
)
	: host(host),
	  port(port),
	  serverName(serverName),
	  clientMaxBodySize(clientMaxBodySize),
	  errorPages(errorPages),
	  locations(locations),
	  defaultServer_(defaultServer) // Updated initialization
{
	if (host.empty()) {
		throw std::invalid_argument("Host is empty");
	}

	if (!(1 <= port && port <= 65535)) {
		throw std::invalid_argument("Invalid port number");
	}

	if (serverName.empty()) {
		throw std::invalid_argument("Server name is empty");
	}

	if (clientMaxBodySize < 1) {
		throw std::invalid_argument("Invalid clientMaxBodySize");
	}

	if (locations.empty()) {
		throw std::invalid_argument("Locations is empty");
	}
}

std::string ServerConfig::getHost() const {
	return host;
}

int ServerConfig::getPort() const {
	return port;
}

std::string ServerConfig::getServerName() const {
	return serverName;
}

size_t ServerConfig::getClientMaxBodySize() const {
	return clientMaxBodySize;
}

const std::vector<LocationConfig>& ServerConfig::getLocations() const {
    return locations;
}

const std::map<int, std::string>& ServerConfig::getErrorPages() const {
	return errorPages;
}

bool ServerConfig::isDefault() const {
	return defaultServer_; // Updated to return the renamed member variable
}
