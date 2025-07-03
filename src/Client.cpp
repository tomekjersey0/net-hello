#include "../include/Client.hpp"
#include <iostream>
#include <string>
#include "GetError.hpp"

Client::Client()
{
    Connect();
    while (true)
    {
        char msg[256];
        int bytes = Recv(sockfd, msg, sizeof(msg));
        if (bytes <= 0)
        {
            break; // stop on error or closed
        }
        std::cout << msg;
        std::string message;
        std::getline(std::cin, message);
        message += "\n";
        Send(sockfd, message.c_str());
    }
}

void Client::Connect()
{
    if (connect(sockfd, (const sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cerr << GetError::getFullMessage(WSAGetLastError()) << std::endl;
        return;
    }
}

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    Client client;

    WSACleanup();
    return 0;
}
