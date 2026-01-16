#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "raii/SafeFile.hpp"

constexpr int FAILED_TO_OPEN = -1;

SafeFile::SafeFile(std::string &RefFilePath){
    fd = open(RefFilePath.c_str(), O_RDWR);
}

bool SafeFile::IsOpen(){
    return(fd!=FAILED_TO_OPEN);
}

std::string SafeFile::Read(){
    std::string returnString;
    if(fd != FAILED_TO_OPEN){
        char ch;
    
        // Read one character at a time until newline or EOF
        while (read(fd, &ch, 1) == 1) {
            if (ch == '\n') {
                break;
            }
            returnString += ch;
        }
    }
    return(returnString);
}

SafeFile::~SafeFile(){
    if(fd != FAILED_TO_OPEN){
        close(fd);
    }
}




