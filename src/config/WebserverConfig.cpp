#include "WebserverConfig.hpp"

WebserverConfig::WebserverConfig(HTTPConfig* httpConfig)
	: httpConfig(httpConfig) {}

HTTPConfig* WebserverConfig::getHTTPConfig() const {
	return const_cast<HTTPConfig*>(httpConfig);
}

