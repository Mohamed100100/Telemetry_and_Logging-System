#include <string>
#include <fstream>
#include <iostream>
#include "sinks/FileSinkImpl.hpp"

FileSinkImpl::FileSinkImpl(std::string &RefFilePath) : FilePath(RefFilePath){}

void FileSinkImpl::write(LogMessage log_message) {
    std::ofstream outFile(FilePath.c_str());

    if(outFile.is_open() == true){
        outFile<<log_message.ToString();
        outFile.close();
    }else{
        std::cerr << "Error: Could not open file for writing : " << FilePath <<  std::endl;
    }
}

