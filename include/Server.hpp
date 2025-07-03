#pragma once
#include "SockAddr.hpp"
#include <vector>
#include <memory>
#include <mutex>

class Server : public SockAddr
{
private:
    struct ClientData
    {
        SOCKET socket;
        sockaddr_in client_addr;
    };
    std::vector<std::shared_ptr<ClientData>> clientData;
    void handleConnection(std::shared_ptr<ClientData> clientData);
    std::mutex clientDataMutex;

public:
    ~Server();

    void Start();

    int Bind();

    void Listen();

    SOCKET Accept(ClientData* clientData);
};
