#pragma once
#include "SockAddr.hpp"
#include <vector>
#include <memory>
#include <mutex>
#include <string>
#include "NetSocket.hpp"

class Server : public SockAddr
{
private:
    struct ClientData
    {
        Net::socket_t socket;
        sockaddr_in client_addr;
        std::string username;
        std::shared_ptr<ClientData> chattingWith;
    };

    struct CommandInfo {
        std::string name;
        std::string description;
    };

    std::vector<CommandInfo> commandsList;

    std::vector<std::shared_ptr<ClientData>> clientData;
    std::mutex clientDataMutex;

    void handleConnection(std::shared_ptr<ClientData> clientData);

    bool RecvFromClient(ClientData *_clientData, char *buffer, size_t buffer_size);

    /* Input sanitization help function for parsing commands */
    std::string trim(const std::string& s);
    bool isCommand(std::string input);

    /* Specific command functions */
    void listUsers(std::shared_ptr<ClientData> client);
    void selectUser(std::shared_ptr<ClientData> client, const std::string& input);
    void quitChat(std::shared_ptr<ClientData> client);
    void helpCommands(std::shared_ptr<ClientData> client);

    void handleCommand(std::shared_ptr<ClientData> client, const std::string& input);

public:
    Server();

    ~Server();

    void Start();

    int Bind();

    void Listen();

    Net::socket_t Accept(ClientData *clientData);
};
