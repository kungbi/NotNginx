#include "Webserver.hpp"
#include "WebserverConfig.hpp"
#include "ServerConfig.hpp"
#include "HTTPConfig.hpp"
#include "LocationConfig.hpp"

#include "ConfigReader.hpp"
#include "ConfigParser.hpp"
#include "ConfigAdapter.hpp"
#include "RequestHandler.hpp"

#include "CgiHandler.hpp"

#include <memory>

WebserverConfig* initializeConfig(std::string configPath) {
    ConfigReader reader;
    std::string configContent = reader.readConfigFile(configPath);

    if (configContent.empty()) throw std::runtime_error("Failed to read configuration file.");

    ConfigParser parser;
    parser.tokenize(configContent);
    IConfigContext* rootContext = parser.parseConfig();
    std::unique_ptr<IConfigContext> rootGuard(rootContext);

    IConfigContext* httpContext = rootContext->getChild()[0];

    if (!httpContext) throw std::runtime_error("Failed to parse configuration.");
    
    ConfigData configData(httpContext);
    HTTPConfig *httpConfig = ConfigAdapter::convertToHTTPConfig(configData);
    return new WebserverConfig(httpConfig);
}

Webserver* dependencyInjection(WebserverConfig* config) {
	Kqueue* kqueue = new Kqueue();
	Servers* servers = new Servers(*kqueue);

	const std::vector<ServerConfig*>* serverConfigs = config->getHTTPConfig()->getServers();
	for (std::vector<ServerConfig*>::const_iterator it = serverConfigs->begin(); it != serverConfigs->end(); ++it) {
		ServerConfig* serverConfig = *it;
		Socket* serverSocket = new Socket(serverConfig->getHost(), serverConfig->getPort());
		Router* router = new Router(*serverConfig);
		CgiHandler* cgiHandler = new CgiHandler(serverSocket->getSocketFd(), *kqueue);
		RequestHandler* requestHandler = new RequestHandler(*router, *cgiHandler);
		Server* server = servers->createServer(*serverSocket, *serverConfig, *kqueue, *requestHandler);
		kqueue->addEvent(server->getSocketFd(), KQUEUE_EVENT::SERVER, server->getSocketFd());
		servers->addServer(*server);
	}

	return new Webserver(*kqueue, *servers, *config);
}

int main(int argc, char* argv[]) {
    WebserverConfig* config = nullptr;
    Webserver* webserver = nullptr;
    try {
        if (2 < argc)
            throw std::runtime_error("Invalid number of arguments");

        std::string configPath = "default.conf";
        if (2 == argc)
            configPath = argv[1];

        config = initializeConfig(configPath);
        webserver = dependencyInjection(config);
        webserver->start();
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
    }

    // 안전하게 메모리 해제
    delete webserver;
    delete config;
    return 0;
}