#pragma once
#include "NetSocket.hpp"
#include <string>

class SockAddr
{
protected:
    // data related to the socket and address information of the MAIN SERVER being connected to
    // this default configuration is set in the default constructor delegation for SockAddr
    // all classes using / connecting to the server socket should extend this class
    struct sockaddr_in addr;
    Net::socket_t sockfd;

public:
    SockAddr();

    SockAddr(int port, const char *ip_addr);

    ~SockAddr();

    int Send(Net::socket_t to, const std::string& msg);

    int Recv(Net::socket_t socket, char* out, size_t buf_size);
};