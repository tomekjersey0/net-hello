#include "GetError.hpp"

/*

    The Winsock implemenation of this class
    POSIX implementations can be found the in Net::NetError class ("NetSocket.hpp")
*/

std::map<int, GetError::Error> GetError::errors = {
    {10004, {10004, "WSAEINTR", "Interrupted function call"}},
    {10009, {10009, "WSAEBADF", "Bad file number"}},
    {10013, {10013, "WSAEACCES", "Permission denied"}},
    {10014, {10014, "WSAEFAULT", "Bad address"}},
    {10022, {10022, "WSAEINVAL", "Invalid argument"}},
    {10024, {10024, "WSAEMFILE", "Too many open files"}},
    {10035, {10035, "WSAEWOULDBLOCK", "Resource temporarily unavailable (non-blocking)"}},
    {10060, {10060, "WSAETIMEDOUT", "Connection timed out"}},
    {10061, {10061, "WSAECONNREFUSED", "Connection refused"}},
    {10054, {10054, "WSAECONNRESET", "Connection reset by peer"}}
};

std::string GetError::getName(int code) {
    auto it = errors.find(code);
    if (it != errors.end()) return it->second.name;
    return "UNKNOWN_ERROR";
}

std::string GetError::getMeaning(int code) {
    auto it = errors.find(code);
    if (it != errors.end()) return it->second.meaning;
    return "No description available";
}

std::string GetError::getFullMessage(int code) {
    auto it = errors.find(code);
    if (it != errors.end())
        return it->second.name + ": " + it->second.meaning;
    return "UNKNOWN_ERROR: No description available";
}