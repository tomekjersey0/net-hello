#include "../include/Server.hpp"
#include <iostream>
#include <cstdint>
#include <thread>
#include <mutex>
#include <iterator>
#include "GetError.hpp"
#include "NetSocket.hpp"

void Server::Start()
{
    if (Bind() == SOCKET_ERROR)
    {
        std::cerr << "Bind failed with error: " << Net::getLastError() << std::endl;
        return;
    }

    Listen();

    while (true)
    {
        auto client = std::make_shared<ClientData>();

        Net::socket_t clientSock = Accept(client.get());
        if (clientSock == INVALID_SOCKET)
        {
            std::cerr << "Accept failed: " << Net::getLastError() << std::endl;
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

Server::Server()
{
    commandsList = {
        {"LIST", "Print who is actively online"},
        {"SELECT", "Send a request to Chat with SELECT username"},
        {"QUIT", "Exit the current chat"},
        {"HELP", "Show list of commands"},
    };
}

Server::~Server()
{
    for (std::shared_ptr<ClientData> client : clientData)
    {
        Net::closeSocket(client->socket);
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

Net::socket_t Server::Accept(ClientData *clientData)
{
    int client_len = sizeof(clientData->client_addr);
    Net::socket_t clientSocket;
    while (true)
    {
        clientSocket = accept(sockfd, (sockaddr *)&clientData->client_addr, &client_len);
        if (clientSocket != INVALID_SOCKET)
        {
            // init other no-val members
            clientData->chattingWith = nullptr;
            clientData->username = "";
            break; // success
        }

        int err = Net::getLastError();

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

bool Server::RecvFromClient(ClientData *_clientData, char *buffer, size_t buffer_size)
{
    Net::socket_t socket = _clientData->socket;
    int bytes = Recv(socket, buffer, buffer_size);
    std::cout << "bytes: " << bytes << std::endl;
    if (bytes < 0)
    {
        // Client most likely disconnected
        return false;
    }
    else if (bytes == 0)
    {
        // Some other error, just disconnect if you can
        return false;
    }
    else
    {
        if (bytes == strlen(buffer))
        {
            buffer[bytes - 1] = '\0';
        }
        else
        {
            buffer[bytes] = '\0';
        }
        return true;
    }
}

std::string Server::trim(const std::string &s)
{
    // Removes trailing and preceding whitespace from a string
    size_t start = s.find_first_not_of(" \r\n\t");
    size_t end = s.find_last_not_of(" \r\n\t");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

bool Server::isCommand(std::string input)
{
    input = trim(input);
    for (auto it = commandsList.begin(); it != commandsList.end(); ++it)
    {
        if (input.rfind(it->name, 0) == 0)
        {
            return true;
        }
    }
    return false;
}

void Server::listUsers(std::shared_ptr<ClientData> client)
{
    std::string message = "";
    for (auto it = clientData.begin(); it != clientData.end(); ++it)
    {
        size_t index = std::distance(clientData.begin(), it);
        std::string out = "[" + std::to_string(index + 1) + "] " + (*it)->username + '\n';
        message += out;
    }
    Send(client->socket, message);
}

void Server::selectUser(std::shared_ptr<ClientData> client, const std::string &input)
{
}

void Server::quitChat(std::shared_ptr<ClientData> client)
{
}

void Server::helpCommands(std::shared_ptr<ClientData> client)
{
    std::string msg = "";
    for (auto it = commandsList.begin(); it != commandsList.end(); ++it)
    {
        msg += it->name + " - " + it->description + "\n";
    }
    Send(client->socket, msg);
}

void Server::handleCommand(std::shared_ptr<ClientData> client, const std::string &input)
{
    /* could add function pointers to the commands list, but really i think its
     more clear to just keep adding these manually to both the commands list and
     here and write custom functions maybe?
    */
    if (input.rfind("LIST", 0) == 0)
    {
        listUsers(client);
    }
    else if (input.rfind("SELECT", 0) == 0)
    {
        selectUser(client, input);
    }
    else if (input.rfind("QUIT", 0) == 0)
    {
        quitChat(client);
    }
    else if (input.rfind("HELP", 0) == 0)
    {
        helpCommands(client);
    }
    else
    {
        Send(client->socket, "Unknown command\n");
    }
}

void Server::handleConnection(std::shared_ptr<ClientData> _clientData)
{
    bool selected = false;
    Send(_clientData->socket, "Welcome to EchoNet! Enter your username:\n");
    while (!selected)
    {
        std::string buffer(256, '\0');
        bool noError = RecvFromClient(_clientData.get(), &buffer[0], buffer.size());
        if (!noError)
        {
            break;
        }
        std::string message = trim(buffer);
        _clientData.get()->username = message;
        selected = true;
    }

    Send(_clientData->socket, "Welcome to EchoNet! Type HELP for a list of COMMANDS.\n");
    while (selected)
    {
        // input text from client
        std::string buffer(256, '\0');
        bool noError = RecvFromClient(_clientData.get(), &buffer[0], buffer.size());
        if (!noError)
        {
            break;
        }
        if (isCommand(buffer))
        {
            std::cout << "yep command here" << std::endl;
            handleCommand(_clientData, buffer);
        }
        else
        {
            std::cout << "NOO command.. " << buffer << std::endl;
        }
        /*
            Temp testing aka 8 ball game

        const char *msgs[] = {"yes", "no", "maybe", "definitely", "possibly", "perhaps", "impossible", "certainly"};

        size_t size_msgs = sizeof(msgs) / sizeof(msgs[0]);
        DWORD time = timeGetTime();
        std::string msg = msgs[time % size_msgs];
        msg += "\n";
        Send(_clientData->socket, msg.c_str());
        */
    }

    // free socket, and remove itself from the main vector
    {
        std::lock_guard<std::mutex> lock(clientDataMutex);
        for (auto it = this->clientData.begin(); it != this->clientData.end(); ++it)
        {
            if (*it == _clientData)
            {
                Net::closeSocket(_clientData->socket);
                this->clientData.erase(it);
                break;
            }
        }
    }
}

int main()
{
    if (!Net::startup())
    {
        std::cerr << "Network init failed\n";
        return 1;
    }

    Server server;
    server.Start();

    Net::cleanup();
    return 0;
}
