#pragma once 

#include "ITelemetrySource.hpp"
#include "SomeIPTelemetrySourceImpl.hpp"

class SomeIPTelemetrySourceAdapter : public ITelemetrySource{
    private :
        SomeIPTelemetrySourceImpl& impl_;

    public :
        SomeIPTelemetrySourceAdapter();
        ~SomeIPTelemetrySourceAdapter() = default;

        SomeIPTelemetrySourceAdapter(const SomeIPTelemetrySourceAdapter& other) = delete;
        SomeIPTelemetrySourceAdapter(SomeIPTelemetrySourceAdapter&& other) = default;

        SomeIPTelemetrySourceAdapter&operator=(const SomeIPTelemetrySourceAdapter& other) = delete;
        SomeIPTelemetrySourceAdapter&operator=(SomeIPTelemetrySourceAdapter&& other) = default;

        virtual bool openSource();
        virtual bool readSource(std::string &RefRead);
};