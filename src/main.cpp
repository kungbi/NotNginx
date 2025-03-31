#include "Webserver.hpp"
#include "WebserverConfig.hpp"
#include "ServerConfig.hpp"
#include "HTTPConfig.hpp"
#include "LocationConfig.hpp"

#include "ConfigReader.hpp"
#include "ConfigParser.hpp"
#include "ConfigAdapter.hpp"
#include "RequestHandler.hpp"

WebserverConfig* initializeConfig(std::string configPath)
{
    // 설정 파일을 읽기
    ConfigReader reader;
    std::string configContent = reader.readConfigFile(configPath);  // 설정 파일 경로

    if (configContent.empty()) throw std::runtime_error("Failed to read configuration file.");

    // 설정을 파싱하여 트리 구조 생성
    ConfigParser parser;
    parser.tokenize(configContent);
    IConfigContext* rootContext = parser.parseConfig();

    if (!rootContext) throw std::runtime_error("Failed to parse configuration.");

    // 트리를 HTTPConfig 객체로 변환
    ConfigData configData(rootContext);
    HTTPConfig httpConfig = ConfigAdapter::convertToHTTPConfig(configData);

    return new WebserverConfig(httpConfig);
}

#include <stdlib.h>

void leak() {
	system("leaks webserv");
}

Webserver* dependencyInjection(WebserverConfig* config) {
	Kqueue* kqueue = new Kqueue(1024);
	std::cout << "Kqueue initialized." << std::endl;

	Servers* servers = new Servers(*kqueue);
	std::cout << "Servers initialized." << std::endl;

	const std::vector<ServerConfig*>& serverConfigs = config->getHTTPConfig().getServers();
	std::cout << "dependencyInjection: Found " << serverConfigs.size() << " servers in HTTPConfig." << std::endl;

	for (std::vector<ServerConfig*>::const_iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it) {
		ServerConfig* serverConfig = *it;
		std::cout << "Creating server for: " << serverConfig->getServerName() << ":" << serverConfig->getPort() << std::endl;

		Socket* serverSocket = new Socket(serverConfig->getHost(), serverConfig->getPort());
		std::cout << "Socket created for: " << serverConfig->getHost() << ":" << serverConfig->getPort() << std::endl;

		Router* router = new Router(*serverConfig);
		std::cout << "Router created for server: " << serverConfig->getServerName() << std::endl;

		RequestHandler* requestHandler = new RequestHandler(*router);
		std::cout << "RequestHandler created for server: " << serverConfig->getServerName() << std::endl;

		Server* server = servers->createServer(*serverSocket, *serverConfig, *kqueue, *requestHandler);
		std::cout << "Server created for: " << serverConfig->getServerName() << ":" << serverConfig->getPort() << std::endl;

		kqueue->addEvent(server->getSocketFd(), KQUEUE_EVENT::SERVER, server->getSocketFd());
		std::cout << "Event added for server FD: " << server->getSocketFd() << std::endl;

		servers->addServer(*server);
		std::cout << "Server added to Servers list." << std::endl;
	}

	std::cout << "All servers initialized." << std::endl;
	return new Webserver(*kqueue, *servers, *config);
}

int main(int argc, char* argv[]) {
	atexit(leak);
	if (2 < argc)
		throw std::runtime_error("Invalid number of arguments");

	std::string configPath = "default.conf";
	if (2 == argc)
		configPath = argv[1];
	
	WebserverConfig* config = initializeConfig(configPath);
	Webserver* webserver = dependencyInjection(config);
	webserver->start();

	delete webserver; // 메모리 해제
	delete config; // 메모리 해제
	return 0;
}
