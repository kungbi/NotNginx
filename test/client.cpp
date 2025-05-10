#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void runClient(const std::string& ip, int port) {
    // 소켓 생성
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        return;
    }

    // 서버 주소 설정
    sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0) {
        close(clientSocket);
        return;
    }

    // 서버에 연결
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        close(clientSocket);
        return;
    }

    std::cout << "Connected to server: " << ip << ":" << port << std::endl;

	// sleep(10);

    // HTTP 요청 전송
    std::string request = "GET / HTTP/1.1\r\n"
                          "Host: " + ip + "\r\n"
                          "Connection: close\r\n"
                          "\r\n";

    if (send(clientSocket, request.c_str(), request.size(), 0) == -1) {
        close(clientSocket);
        return;
    }
    std::cout << "Request sent to server." << std::endl;

    // 서버 응답 수신
    char buffer[4096];
    ssize_t bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0'; // null-terminate
        std::cout << buffer;
    }

    if (bytesReceived == -1) {
    }

    // 소켓 종료
    close(clientSocket);
    std::cout << "Connection closed." << std::endl;
}

int main() {
    std::string serverIp = "127.0.0.1"; // 테스트 서버 IP
    int serverPort = 8080;             // 테스트 서버 포트

    runClient(serverIp, serverPort);

    return 0;
}
