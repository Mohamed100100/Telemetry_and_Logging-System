
#include <memory>

#include "sources/ITelemetrySource.hpp"
#include "raii/SafeFile.hpp"

class FileTelemetrySourceImpl : public ITelemetrySource{
    private :
        std::string FilePath;
        std::unique_ptr<SafeFile> _safeFilePtr;

    public :
        FileTelemetrySourceImpl() = delete;
        FileTelemetrySourceImpl(std::string &FilePath);
        FileTelemetrySourceImpl(const FileTelemetrySourceImpl& other) = delete;
        FileTelemetrySourceImpl(FileTelemetrySourceImpl&& other) = default;

        FileTelemetrySourceImpl &operator=(const FileTelemetrySourceImpl & other) = delete;
        FileTelemetrySourceImpl &operator=(FileTelemetrySourceImpl && other) = default;

        virtual bool openSource();
        virtual bool readSource(std::string &RefRead);

        virtual~FileTelemetrySourceImpl() = default;

};