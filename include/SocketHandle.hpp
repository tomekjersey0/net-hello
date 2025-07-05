#pragma once

#include "NetSocket.hpp"
#include <string>
#include <string>


class SocketHandle
{
protected:
    Net::socket_t sockfd;
    sockaddr_in addr;

public:
    SocketHandle();
    SocketHandle(int port, const char* ip_addr);
    ~SocketHandle();

    int Send(Net::socket_t to, const std::string& msg);
    int Recv(Net::socket_t socket, char* out, size_t buf_size);

    Net::socket_t getSocket() const { return sockfd; }
    sockaddr_in* getAddr() { return &addr; }
};