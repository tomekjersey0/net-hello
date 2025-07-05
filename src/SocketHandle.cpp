#include "SocketHandle.hpp"
#include <cstring>
#include <iostream>

SocketHandle::SocketHandle() : SocketHandle(55555, "127.0.0.1") {}

SocketHandle::SocketHandle(int port, const char* ip_addr)
{
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip_addr);
}

SocketHandle::~SocketHandle()
{
    Net::closeSocket(sockfd);
    shutdown(sockfd, Net::SHUTDOWN_BOTH);
}

int SocketHandle::Send(Net::socket_t to, const std::string& msg)
{
    return send(to, msg.c_str(), msg.size(), 0) == msg.size();
}

int SocketHandle::Recv(Net::socket_t socket, char* out, size_t buf_size)
{
    int bytes = recv(socket, out, buf_size, 0);
    if (bytes <= 0)
    {
        std::cerr << Net::NetError::lastErrorMessage() << std::endl;
    }
    else
    {
        if (bytes < buf_size)
            out[bytes] = '\0';
        else
            out[buf_size - 1] = '\0';
    }
    return bytes;
}
