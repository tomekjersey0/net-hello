#ifndef NET_SOCKET_HPP
#define NET_SOCKET_HPP

// Standard C++ includes
#include <cstdint>
#include <cstring>

// Platform-specific includes
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <netinet/in.h>
    #include <netdb.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
#endif

namespace Net {

    // --- Common typedefs ---
#ifdef _WIN32
    using socket_t   = SOCKET;
    using socklen_t  = int; // Windows doesn't define socklen_t
    constexpr socket_t INVALID_SOCKET_VALUE = INVALID_SOCKET;
    constexpr int SOCKET_ERROR_VALUE = SOCKET_ERROR;
#else
    using socket_t   = int;
    using socklen_t  = ::socklen_t;
    constexpr socket_t INVALID_SOCKET_VALUE = -1;
    constexpr int SOCKET_ERROR_VALUE = -1;
#endif

    // --- Shutdown constants ---
#ifdef _WIN32
    constexpr int SHUTDOWN_BOTH = SD_BOTH;
    constexpr int SHUTDOWN_READ = SD_RECEIVE;
    constexpr int SHUTDOWN_WRITE = SD_SEND;
#else
    constexpr int SHUTDOWN_BOTH = SHUT_RDWR;
    constexpr int SHUTDOWN_READ = SHUT_RD;
    constexpr int SHUTDOWN_WRITE = SHUT_WR;
#endif

    // --- Startup and cleanup ---
    inline bool startup() {
#ifdef _WIN32
        WSADATA wsaData;
        return WSAStartup(MAKEWORD(2,2), &wsaData) == 0;
#else
        return true; // No-op for POSIX
#endif
    }

    inline void cleanup() {
#ifdef _WIN32
        WSACleanup();
#endif
    }

    // --- Close socket ---
    inline int closeSocket(socket_t s) {
#ifdef _WIN32
        return closesocket(s);
#else
        return close(s);
#endif
    }

    // --- Get last error ---
    inline int getLastError() {
#ifdef _WIN32
        return WSAGetLastError();
#else
        return errno;
#endif
    }

    // --- Non-blocking mode example ---
    inline int setNonBlocking(socket_t s, bool nonBlocking) {
#ifdef _WIN32
        u_long mode = nonBlocking ? 1 : 0;
        return ioctlsocket(s, FIONBIO, &mode);
#else
        int flags = fcntl(s, F_GETFL, 0);
        if (flags == -1) return -1;
        if (nonBlocking)
            flags |= O_NONBLOCK;
        else
            flags &= ~O_NONBLOCK;
        return fcntl(s, F_SETFL, flags);
#endif
    }

} // namespace Net

#endif // NET_SOCKET_HPP
