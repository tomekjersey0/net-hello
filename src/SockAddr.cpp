#include "../include/SockAddr.hpp"
#include <cstring>
#include <iostream>

SockAddr::SockAddr() : SockAddr(55555, "127.0.0.1") {}

SockAddr::SockAddr(int port, const char *ip_addr)
{
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip_addr);
}

SockAddr::~SockAddr()
{
    closesocket(sockfd);
}

int SockAddr::Send(SOCKET to, const char *msg)
{
    return send(to, msg, strlen(msg), 0);
}

int SockAddr::Recv(SOCKET socket, char* out, size_t buf_size)
{
    int bytes = recv(socket, out, buf_size, 0);
    if (bytes == 0) {
        std::cout << "Peer closed connection." << std::endl;
    } else if (bytes < 0) {
        std::cout << "Error occured." << std::endl;
    } else {
        if (bytes < buf_size) {
            out[bytes] = '\0';
        } else {
            out[buf_size - 1] = '\0';
        }
    }
    return bytes;
}