

#pragma once

#include "logger/LogMessage.hpp"
#include "sinks/ILogSink.hpp"


class FileSinkImpl : public ILogSink{
    private:
        std::string FilePath;
    public:
        FileSinkImpl() = delete;
        FileSinkImpl(std::string &RefFilePath);
        
        FileSinkImpl(const FileSinkImpl& other) = delete;
        FileSinkImpl(FileSinkImpl &&other) = delete;

        FileSinkImpl & operator =(const FileSinkImpl& other) = delete;
        FileSinkImpl & operator =(FileSinkImpl && other) = default;

        virtual void write(const LogMessage &log_message);

        virtual ~FileSinkImpl() = default;
};

