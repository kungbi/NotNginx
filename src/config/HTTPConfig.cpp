#include "HTTPConfig.hpp"

// Constructor definition
HTTPConfig::HTTPConfig(const std::vector<ServerConfig*>& servers) {
	for (size_t i = 0; i < servers.size(); ++i) {
		this->servers.push_back(new ServerConfig(*servers[i])); // Deep copy each ServerConfig
	}
	std::cout << "HTTPConfig initialized with " << this->servers.size() << " servers." << std::endl;
}

// Destructor to free memory
HTTPConfig::~HTTPConfig() {
	for (size_t i = 0; i < servers.size(); ++i) {
		delete servers[i]; // Free each ServerConfig pointer
	}
	servers.clear();
}

// Copy constructor for deep copy
HTTPConfig::HTTPConfig(const HTTPConfig& other) {
	for (size_t i = 0; i < other.servers.size(); ++i) {
		this->servers.push_back(new ServerConfig(*other.servers[i])); // Deep copy each ServerConfig
	}
}

// Assignment operator for deep copy
HTTPConfig& HTTPConfig::operator=(const HTTPConfig& other) {
	if (this != &other) {
		// Free existing memory
		for (size_t i = 0; i < servers.size(); ++i) {
			delete servers[i];
		}
		servers.clear();

		// Deep copy from other
		for (size_t i = 0; i < other.servers.size(); ++i) {
			this->servers.push_back(new ServerConfig(*other.servers[i]));
		}
	}
	return *this;
}

// Getter for servers
const std::vector<ServerConfig*>& HTTPConfig::getServers() const {
	std::cout << "Returning " << servers.size() << " servers from HTTPConfig." << std::endl;
	return servers;
}