
#include "sources/FileTelemetrySourceImpl.hpp"

FileTelemetrySourceImpl::FileTelemetrySourceImpl(std::string &RefFilePath) : FilePath(RefFilePath){
    _safeFilePtr = nullptr;                        
}
            

bool FileTelemetrySourceImpl::openSource(){
    _safeFilePtr.reset(new SafeFile(FilePath));
    return(_safeFilePtr->IsOpen());
}


bool FileTelemetrySourceImpl::readSource(std::string &RefRead){
    bool ReturnState;
    if(_safeFilePtr != nullptr){
        if(_safeFilePtr->IsOpen() == true){
            RefRead = _safeFilePtr->Read();
            ReturnState = true;
        }else{
            ReturnState = false;
        }
    }else{
        ReturnState = false;
    }
    return(ReturnState);
}

