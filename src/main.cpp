#include "Webserver.hpp"
#include "WebserverConfig.hpp"
#include "ServerConfig.hpp"
#include "HTTPConfig.hpp"
#include "LocationConfig.hpp"

#include "ConfigReader.hpp"
#include "ConfigParser.hpp"
#include "ConfigAdapter.hpp"

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
	Servers* servers = new Servers(*kqueue);
	for (std::vector<ServerConfig>::const_iterator it = config->getHTTPConfig().getServers().begin(); it != config->getHTTPConfig().getServers().end(); ++it) {
		ServerConfig serverConfig = *it;
		Socket* serverSocket = new Socket(serverConfig.getHost(), serverConfig.getPort());
		Server* server = servers->createServer(*serverSocket, serverConfig, *kqueue);

		kqueue->addEvent(server->getSocketFd(), KQUEUE_EVENT::SERVER, server->getSocketFd());
		servers->addServer(*server);
	}

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
