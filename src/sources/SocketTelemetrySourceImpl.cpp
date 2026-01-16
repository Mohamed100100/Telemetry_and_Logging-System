

#include "sources/SocketTelemetrySourceImpl.hpp"


SocketTelemetrySourceImpl::SocketTelemetrySourceImpl(std::string &RefFilePath) : FilePath(RefFilePath){
    _safeSocketPtr = nullptr;
}


bool SocketTelemetrySourceImpl::openSource(){
    _safeSocketPtr.reset(new SafeSocket(FilePath));
    return(_safeSocketPtr->IsOpen());
}

bool SocketTelemetrySourceImpl::readSource(std::string &RefRead){
    bool ReturnState;
    if(_safeSocketPtr != nullptr){
        if(_safeSocketPtr->IsOpen() == true){
            RefRead = _safeSocketPtr->Read();
            ReturnState = true;
        }else{
            ReturnState = false;
        }
    }else{
        ReturnState = false;
    }
    return(ReturnState);
}



