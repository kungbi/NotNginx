#include "WebserverConfig.hpp"

WebserverConfig::WebserverConfig(const HTTPConfig& httpConfig)
	: httpConfig(httpConfig) {}

HTTPConfig& WebserverConfig::getHTTPConfig() const {
	return const_cast<HTTPConfig&>(httpConfig);
}

void WebserverConfig::printConfig() const {
	std::cout << "WebserverConfig Details:" << std::endl;

	// Iterate over the servers in HTTPConfig
	const std::vector<ServerConfig>& servers = httpConfig.getServers();
	for (size_t i = 0; i < servers.size(); ++i) {
		const ServerConfig& server = servers[i];
		std::cout << "Server " << i + 1 << ":" << std::endl;
		std::cout << "  Host: " << server.getHost() << std::endl;
		std::cout << "  Port: " << server.getPort() << std::endl;
		std::cout << "  Server Name: " << server.getServerName() << std::endl;
		std::cout << "  Client Max Body Size: " << server.getClientMaxBodySize() << std::endl;

		std::cout << "  Error Pages:" << std::endl;
		const std::map<int, std::string>& errorPages = server.getErrorPages();
		for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
			std::cout << "    Error Code: " << it->first << ", File: " << it->second << std::endl;
		}

		std::cout << "  Locations:" << std::endl;
		const std::vector<LocationConfig>& locations = server.getLocations();
		for (size_t j = 0; j < locations.size(); ++j) {
			std::cout << "    Location " << j + 1 << ": " << locations[j].getPath() << std::endl;
		}

		std::cout << "  Is Default: " << (server.isDefault() ? "Yes" : "No") << std::endl; // Updated to use the getter
	}
}
