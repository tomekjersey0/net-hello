#include "Client.hpp"
#include <iostream>
#include <string>
#include <thread>
#include "NetSocket.hpp"

void Client::handleUserInput()
{
    while (running)
    {
        std::string message;
        std::getline(std::cin, message);
        if (message.empty())
            continue;

        if (message == "EXIT")
        {
            running = false;
            // closes the socket completly, preventing the Recv in handleRecv from blocking and stalling exit
            shutdown(sockfd, Net::SHUTDOWN_BOTH);
            break;
        }

        message += '\n';
        Client::Send(sockfd, message);
    }
}

void Client::handleRecv()
{
    while (running)
    {
        std::string buffer(256, '\0');
        int bytes = Recv(sockfd, &buffer[0], buffer.size());
        if (bytes <= 0)
        {
            std::cerr << Net::NetError::lastErrorMessage() << std::endl;
            std::cerr << "bytes: " << bytes << std::endl;
            break;
        }
        buffer.resize(bytes); // truncate to actual received size
        std::cout << buffer;
    }
}

Client::Client()
{
    Connect();
    inputThread = std::thread(&Client::handleUserInput, this);
    recvThread = std::thread(&Client::handleRecv, this);
}

void Client::Start()
{
    // block until both threads finish (controlled by atomic running)
    if (inputThread.joinable())
        inputThread.join();
    if (recvThread.joinable())
        recvThread.join();
}

Client::~Client()
{
    running = false;
    Net::closeSocket(sockfd);

    // in case user forgets to run Start, prevents memory leaks from the threads on close
    if (inputThread.joinable())
        inputThread.join();
    if (recvThread.joinable())
        recvThread.join();
}

void Client::Connect()
{
    if (connect(sockfd, (const sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        std::cerr << Net::NetError::lastErrorMessage() << std::endl;
        return;
    }
}

int main()
{
    if (!Net::startup())
    {
        std::cerr << "Network init failed\n";
        return 1;
    }

    Client client;
    client.Start();

    Net::cleanup();
    return 0;
}
