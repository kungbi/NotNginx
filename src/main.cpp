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
	IConfigContext* httpContext = rootContext->getChild()[0]; // 첫 번째 자식이 http 컨텍스트라고 가정

	if (!httpContext) throw std::runtime_error("Failed to parse configuration.");
	
	// 트리를 HTTPConfig 객체로 변환
	ConfigData configData(httpContext);
	HTTPConfig *httpConfig = ConfigAdapter::convertToHTTPConfig(configData);
	delete rootContext; // 메모리 해제
	return new WebserverConfig(httpConfig);
}

#include <stdlib.h>

void leak() {
	system("leaks webserv");
}

Webserver* dependencyInjection(WebserverConfig* config) {
	Kqueue* kqueue = new Kqueue(1024);
	Servers* servers = new Servers(*kqueue);

	const std::vector<ServerConfig*>* serverConfigs = config->getHTTPConfig()->getServers();
	for (std::vector<ServerConfig*>::const_iterator it = serverConfigs->begin(); it != serverConfigs->end(); ++it) {
		ServerConfig* serverConfig = *it;
		Socket* serverSocket = new Socket(serverConfig->getHost(), serverConfig->getPort());
		Router* router = new Router(*serverConfig);
		CgiHandler* cgiHandler = new CgiHandler(*kqueue);
		RequestHandler* requestHandler = new RequestHandler(*router, *cgiHandler);
		Server* server = servers->createServer(*serverSocket, *serverConfig, *kqueue, *requestHandler);
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
