
#include <sys/types.h>          
#include <sys/socket.h>
#include"raii/SafeSocket.hpp"
#include <cstring>
#include <sys/un.h>
#include <unistd.h>


constexpr int FAILED_TO_OPEN = -1;
constexpr int FAILED_TO_CONNECT = -1;


SafeSocket::SafeSocket(std::string &RefFilePath){
   SocketFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (SocketFd != FAILED_TO_OPEN) {
        
        // Setup address structure
        struct sockaddr_un addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        std::strncpy(addr.sun_path, RefFilePath.c_str(), sizeof(addr.sun_path) - 1);
        
        int result = connect(SocketFd, 
                           reinterpret_cast<struct sockaddr*>(&addr), 
                           sizeof(addr));
        if(result == FAILED_TO_CONNECT){
            close(SocketFd);
            SocketFd = FAILED_TO_OPEN;
        }else{
            // The Connection Happen successfully  
        }
    }else{
        SocketFd = FAILED_TO_OPEN;
    }
}

bool SafeSocket::IsOpen(){
    return(SocketFd!=FAILED_TO_OPEN);
}

std::string SafeSocket::Read(){
    std::string returnString;
    if(SocketFd != FAILED_TO_OPEN){
        char ch;
    
        // Read one character at a time until newline or EOF
        while (read(SocketFd, &ch, 1) == 1) {
            if (ch == '\n') {
                break;
            }
            returnString += ch;
        }
    }
    return(returnString);
}

SafeSocket::~SafeSocket(){
    if(SocketFd != FAILED_TO_OPEN){
        close(SocketFd);
    }
}






