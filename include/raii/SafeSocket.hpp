# pragma once 

#include <string>

class SafeSocket{
    private :
        int SocketFd;
    public :
        SafeSocket() = delete;
        SafeSocket(std::string &RefFilePath);
        SafeSocket(SafeSocket&& other) = default;
        SafeSocket(const SafeSocket& other) = delete;

        SafeSocket& operator=(const SafeSocket& other) = delete;
        SafeSocket& operator=(SafeSocket&& other) = default;

        bool IsOpen();
        std::string Read();

        ~SafeSocket();
};