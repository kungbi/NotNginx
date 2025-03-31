#include "WebserverConfig.hpp"

WebserverConfig::WebserverConfig(const HTTPConfig& httpConfig)
	: httpConfig(httpConfig) {}

HTTPConfig& WebserverConfig::getHTTPConfig() const {
	return const_cast<HTTPConfig&>(httpConfig);
}

