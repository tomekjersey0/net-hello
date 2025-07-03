#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>

class SockAddr
{
protected:
    struct sockaddr_in addr;
    SOCKET sockfd;

public:
    SockAddr();

    SockAddr(int port, const char *ip_addr);

    ~SockAddr();

    int Send(SOCKET to, const char *msg);

    int Recv(SOCKET socket, char* out, size_t buf_size);
};