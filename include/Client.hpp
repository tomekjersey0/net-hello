#pragma once
#include "SocketHandle.hpp"
#include <thread>
#include <atomic>

class Client : public SocketHandle
{
private:
    std::atomic<bool> running{true};
    std::thread inputThread;
    std::thread recvThread;

    // thread functions for seperating the user input and recieving data loops
    void handleUserInput();
    void handleRecv();

public:
    Client();
    ~Client();

    void Start();

    void Connect();
};