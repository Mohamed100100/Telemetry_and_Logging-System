

#include <iostream>
#include "sinks/ConsoleSinkImpl.hpp"


void ConsoleSinkImpl::write(LogMessage log_message){
    std::cout<< log_message.ToString() <<std::endl;
}


