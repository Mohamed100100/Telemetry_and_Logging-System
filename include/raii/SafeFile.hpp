# pragma once 

#include <string>

class SafeFile{
    private :
        int fd;
    public :
        SafeFile() = delete;
        SafeFile(std::string &RefFilePath);
        SafeFile(SafeFile&& other) = default;
        SafeFile(const SafeFile& other) = delete;

        SafeFile& operator=(const SafeFile& other) = delete;
        SafeFile& operator=(SafeFile&& other) = default;

        bool IsOpen();
        std::string Read();

        ~SafeFile();
};