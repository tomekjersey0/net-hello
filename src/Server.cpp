#include "../include/Server.hpp"
#include <iostream>
#include <windows.h>
#include <cstdint>
#include <thread>
#include <mutex>
#include "GetError.hpp"

void Server::Start()
{
    if (Bind() == SOCKET_ERROR)
    {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        return;
    }

    while (true)
    {
        Listen();

        auto client = std::make_shared<ClientData>();

        SOCKET clientSock = Accept(client.get());
        if (clientSock == INVALID_SOCKET)
        {
            std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            continue;
        }

        // Store shared_ptr in vector so server holds ownership while thread runs
        {
            std::lock_guard<std::mutex> lock(clientDataMutex);
            clientData.push_back(client); // store it
        }

        // to also give ownership to the thread
        std::thread t(&Server::handleConnection, this, client);
        t.detach();
    }
}

Server::~Server()
{
    for (std::shared_ptr<ClientData> client : clientData)
    {
        closesocket(client->socket);
    }
}

int Server::Bind()
{
    return bind(sockfd, (const sockaddr *)&addr, sizeof(addr));
}

void Server::Listen()
{
    int r = listen(sockfd, SOMAXCONN);
    if (r == 0)
    {
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(addr.sin_addr), ip_str, INET_ADDRSTRLEN);
        uint16_t port = ntohs(addr.sin_port);
        std::cout << "Listening on: " << ip_str << ":" << port << std::endl;
    }
    else
    {
        perror("Failed to listen!");
    }
}

SOCKET Server::Accept(ClientData *clientData)
{
    int client_len = sizeof(clientData->client_addr);
    SOCKET clientSocket;
    while (true)
    {
        clientSocket = accept(sockfd, (sockaddr *)&clientData->client_addr, &client_len);
        if (clientSocket != INVALID_SOCKET)
        {
            break; // success
        }

        int err = WSAGetLastError();

        // Handle specific errors (example)
        if (err == WSAEINTR || err == WSAEWOULDBLOCK)
        {
            Sleep(100); // wait 100 ms before retrying
            continue;
        }

        // For other errors, log and decide what to do
        std::cerr << "accept() failed with error: " << err << std::endl;
        // You could exit, throw, or return INVALID_SOCKET here
        return INVALID_SOCKET;
    }

    clientData->socket = clientSocket;
    std::cout << "Client connected!" << std::endl;
    return clientSocket;
}

void Server::handleConnection(std::shared_ptr<ClientData> clientData)
{
    SOCKET socket = clientData->socket;
    Send(socket, "Welcome to Magic 8 Ball! Ask a question, and you will recieve an answer!\n");
    while (true)
    {
        char buffer[256];
        int bytes = Recv(socket, buffer, sizeof(buffer));
        if (bytes < 0)
        {
            std::cout << GetError::getFullMessage(WSAGetLastError()) << std::endl;
            break;
        }
        else if (bytes == 0)
        {
            std::cout << GetError::getFullMessage(WSAGetLastError()) << std::endl;
            break;
        }
        else
        {
            if (bytes == sizeof(buffer))
            {
                buffer[bytes - 1] = '\0';
            }
            else
            {
                buffer[bytes] = '\0';
            }
            std::cout << buffer;
        }
        /*
            Temp testing
        */
        const char *msgs[] = {"yes", "no", "maybe", "definitely", "possibly", "perhaps", "impossible", "certainly"};

        size_t size_msgs = sizeof(msgs) / sizeof(msgs[0]);
        DWORD time = timeGetTime();
        std::string msg = msgs[time % size_msgs];
        msg += "\n";
        Send(socket, msg.c_str());
    }

    // free socket, and remove itself from the main vector
    {
        std::lock_guard<std::mutex> lock(clientDataMutex);
        for (auto it = this->clientData.begin(); it != this->clientData.end(); ++it)
        {
            if (*it == clientData)
            {
                closesocket(clientData->socket);
                this->clientData.erase(it);
                break;
            }
        }
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

    Server server;

    server.Start();

    WSACleanup();
    return 0;
}
