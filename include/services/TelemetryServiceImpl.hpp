
#pragma once 

#include <memory>
#include "CommonAPI/CommonAPI.hpp"

#include "src-gen/v1/log/TelemetryServiceStubDefault.hpp"

class TelemetryServiceImpl : public v1::log::TelemetryServiceStubDefault {
    public:
        TelemetryServiceImpl();          
        TelemetryServiceImpl(const TelemetryServiceImpl& other) = delete;                          
        TelemetryServiceImpl(TelemetryServiceImpl&& other) = default;
        
        TelemetryServiceImpl& operator=(const TelemetryServiceImpl& other) = delete;
        TelemetryServiceImpl& operator=(TelemetryServiceImpl&& other) = default;

       ~TelemetryServiceImpl() = default;

       void getLoad(const std::shared_ptr<CommonAPI::ClientId> _client, getLoadReply_t _reply);

    private:
        uint8_t GetCpuLoad();
        
};