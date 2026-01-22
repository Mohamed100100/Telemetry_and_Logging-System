

#include <iostream>
#include "sinks/ConsoleSinkImpl.hpp"


void ConsoleSinkImpl::write(const LogMessage &log_message){
    std::cout<< const_cast<LogMessage&>(log_message).ToString() <<std::endl;
}


