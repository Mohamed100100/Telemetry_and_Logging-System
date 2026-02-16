


#include "sources/SomeIPTelemetrySourceAdapter.hpp"


SomeIPTelemetrySourceAdapter::SomeIPTelemetrySourceAdapter():impl_(SomeIPTelemetrySourceImpl::getInstance()){}


bool SomeIPTelemetrySourceAdapter::openSource(){
    return impl_.init() && impl_.connect();
}

bool SomeIPTelemetrySourceAdapter::readSource(std::string& data){                    
    if (!impl_.isConnected()) 
    {
        return false;
    }                
    data = impl_.requestTelemetry();                       
    return !data.empty();                                  
}                        


