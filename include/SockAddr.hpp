#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

class SockAddr
{
protected:
    // data related to the socket and address information of the MAIN SERVER being connected to
    // this default configuration is set in the default constructor delegation for SockAddr
    // all classes using / connecting to the server socket should extend this class
    struct sockaddr_in addr;
    SOCKET sockfd;

public:
    SockAddr();

    SockAddr(int port, const char *ip_addr);

    ~SockAddr();

    int Send(SOCKET to, const std::string& msg);

    int Recv(SOCKET socket, char* out, size_t buf_size);
};