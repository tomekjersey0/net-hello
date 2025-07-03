#pragma once
#include "SockAddr.hpp"

class Client :public SockAddr {
    public:
        Client();

        void Connect();
};