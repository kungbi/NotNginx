#include "ServerConfig.hpp"

ServerConfig::ServerConfig(
	const std::string host,
	const int port,
	const std::string serverName,
	const size_t clientMaxBodySize, // (mb)

	const std::map<int, std::string>& errorPages,
	const std::vector<LocationConfig>& locations,

	const bool isDefault
)
	: host(host),
	  port(port),
	  serverName(serverName),
	  clientMaxBodySize(clientMaxBodySize),
	  errorPages(errorPages),
	  locations(locations),
	  isDefault(isDefault)
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
		throw std::invalid_argument("Invalid clientMaxNodySize");
	}

	if (errorPages.empty()) {
		throw std::invalid_argument("Error pages is empty");
	}

	if (locations.empty()) {
		throw std::invalid_argument("Locations is empty");
	}
}

const int ServerConfig::getPort() const {
	return port;
}

const std::string ServerConfig::getServerName() const {
	return serverName;
}

const size_t ServerConfig::getClientMaxBodySize() const {
	return clientMaxBodySize;
}
