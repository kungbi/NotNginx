#include "HTTPConfig.hpp"

// Constructor definition
HTTPConfig::HTTPConfig(std::vector<ServerConfig*>* servers) {
	this->servers = servers; // Initialize the servers vector with the provided vecto
	std::cout << "HTTPConfig initialized with " << this->servers->size() << " servers." << std::endl;
}

// Destructor to free memory
HTTPConfig::~HTTPConfig() {
	for (size_t i = 0; i < servers->size(); ++i) {
		std::cout << "Deleting server at index " << i << std::endl;
		delete (*servers)[i]; // Delete each ServerConfig object
	}
	servers->clear();
}

// Getter for servers
const std::vector<ServerConfig*>* HTTPConfig::getServers() const {
	std::cout << "Returning " << servers->size() << " servers from HTTPConfig." << std::endl;
	return servers;
}