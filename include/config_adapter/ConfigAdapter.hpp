#ifndef CONFIG_ADAPTER_HPP
#define CONFIG_ADAPTER_HPP

#include "HTTPConfig.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
#include "ConfigData.hpp"

class ConfigAdapter
{
	private:
		static LocationConfig convertToLocationConfig(IConfigContext* locationContext);
		static ServerConfig convertToServerConfig(IConfigContext* serverContext);

	public:
		static HTTPConfig convertToHTTPConfig(ConfigData& configData);
};

#endif
