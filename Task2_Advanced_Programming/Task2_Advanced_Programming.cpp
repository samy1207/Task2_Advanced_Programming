#include <iostream>
#include <string>
#include <cstring>
#include <exception>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

class Comms {
protected:
    WSADATA wsaData;
    SOCKET sockfd;
    struct sockaddr_in serverAddr, clientAddr;

public:
    Comms() {
        // Initialize Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    }

    virtual ~Comms() {
        WSACleanup();
    }

    void bind(int port) {
        // Create socket
        sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sockfd == INVALID_SOCKET) {
            throw std::runtime_error("socket creation failed");
        }

        // Bind to address and port
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(port);
        if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            throw std::runtime_error("bind failed");
        }
    }

    void listen() {
        if (::listen(sockfd, SOMAXCONN) == SOCKET_ERROR) {
            throw std::runtime_error("listen failed");
        }
    }

    SOCKET accept() {
        struct sockaddr_in clientAddr;
        int clientLen = sizeof(clientAddr);
        SOCKET clientfd = ::accept(sockfd, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientfd == INVALID_SOCKET) {
            throw std::runtime_error("accept failed");
        }
        return clientfd;
    }

    void connect(std::string ipAddress, int port) {
        // Create socket
        sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sockfd == INVALID_SOCKET) {
            throw std::runtime_error("socket creation failed");
        }

        // Connect to server
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
        serverAddr.sin_port = htons(port);
        if (::connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            throw std::runtime_error("connect failed");
        }
    }

    void send(char* buffer, int len) {
        if (::send(sockfd, buffer, len, 0) == SOCKET_ERROR) {
            throw std::runtime_error("send failed");
        }
    }

    int recv(char* buffer, int len) {
        int numBytes = ::recv(sockfd, buffer, len, 0);
        if (numBytes == SOCKET_ERROR) {
            throw std::runtime_error("recv failed");
        }
        return numBytes;
    }

    void close() {
        ::closesocket(sockfd);
    }
};

class Server : public Comms {
public:
    Server(int port) {
        bind(port);
        listen();
    }

    void run() {
        std::cout << "Server started" << std::endl;

        // Wait for client connection
        SOCKET clientfd = accept();
        std::cout << "Client connected" << std::endl;

        // Exchange messages with client
        char* buffer = new char[1024];
        while (true) {
            // Receive message from client
            memset(buffer, 0, sizeof(char) * 1024);
            int numBytes = recv(buffer, 1024);
            if (numBytes > 0) {
                std::cout << "Received message: " << buffer << std::endl;
            }
        }

        delete[] buffer;
    }


};
