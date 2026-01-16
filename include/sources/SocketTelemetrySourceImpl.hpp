#include <memory>

#include "core/ITelemetrySource.hpp"
#include "raii/SafeSocket.hpp"

class SocketTelemetrySourceImpl : public ITelemetrySource{
    private :
        std::string FilePath;
        std::unique_ptr<SafeSocket> _safeSocketPtr;

    public :
        SocketTelemetrySourceImpl() = delete;
        SocketTelemetrySourceImpl(std::string &RefFilePath);
        SocketTelemetrySourceImpl(const SocketTelemetrySourceImpl& other) = delete;
        SocketTelemetrySourceImpl(SocketTelemetrySourceImpl&& other) = default;

        SocketTelemetrySourceImpl &operator=(const SocketTelemetrySourceImpl & other) = delete;
        SocketTelemetrySourceImpl &operator=(SocketTelemetrySourceImpl && other) = default;

        virtual bool openSource();
        virtual bool readSource(std::string &RefRead);

        virtual ~SocketTelemetrySourceImpl() = default;
};