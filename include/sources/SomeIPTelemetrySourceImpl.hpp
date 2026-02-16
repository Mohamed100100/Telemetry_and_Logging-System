
#pragma once

#include "CommonAPI/CommonAPI.hpp"
#include "src-gen/v1/log/TelemetryServiceProxy.hpp"


class SomeIPTelemetrySourceImpl {                           
    private:
        std::shared_ptr<CommonAPI::Runtime> runtime_;
        std::shared_ptr<v1::log::TelemetryServiceProxy<>> proxy_;
        bool connected_;
        bool initialized_;
        mutable std::mutex mutex_;  // For thread safety



        SomeIPTelemetrySourceImpl();
        ~SomeIPTelemetrySourceImpl();


    public:
        static SomeIPTelemetrySourceImpl& getInstance();

        SomeIPTelemetrySourceImpl(const SomeIPTelemetrySourceImpl&) = delete;
        SomeIPTelemetrySourceImpl& operator=(const SomeIPTelemetrySourceImpl&) = delete;
        SomeIPTelemetrySourceImpl(SomeIPTelemetrySourceImpl&&) = delete;
        SomeIPTelemetrySourceImpl& operator=(SomeIPTelemetrySourceImpl&&) = delete;

       bool init();                                               
       bool connect();                                            
       void disconnect();                                         
       std::string requestTelemetry();                            
       bool isConnected();
};

