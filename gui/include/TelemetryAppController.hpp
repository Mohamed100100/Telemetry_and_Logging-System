/**
 * @file TelemetryAppController.hpp
 * @brief Controller bridging the GUI with the telemetry backend
 */

#ifndef TELEMETRYAPPCONTROLLER_HPP
#define TELEMETRYAPPCONTROLLER_HPP

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include <QFuture>
#include <memory>
#include <atomic>
#include <map>

#include "ConfigDialog.hpp"
#include "sources/ITelemetrySource.hpp"

// Forward declarations
class LogManager;
class ILogSink;

template<typename Policy>
class LogFormatter;

class CpuPolicy;
class GpuPolicy;
class RamPolicy;

/**
 * @class TelemetryAppController
 * @brief Manages the telemetry application lifecycle and data flow
 */
class TelemetryAppController : public QObject
{
    Q_OBJECT

public:
    explicit TelemetryAppController(QObject *parent = nullptr);
    ~TelemetryAppController();

    bool isRunning() const;
    AppConfigData getConfig() const;
    void setConfig(const AppConfigData &config);
    bool loadConfig(const QString &filePath);
    bool saveConfig(const QString &filePath);

public slots:
    void start();
    void stop();
    void restart();

signals:
    void telemetryReceived(const QString &type, double value, const QString &severity);
    void logReceived(const QString &timestamp, const QString &severity,
                     const QString &context, const QString &message);
    void sourceStatusChanged(const QString &sourceName, bool isActive);
    void started();
    void stopped();
    void errorOccurred(const QString &error);

private slots:
    void pollTelemetry();
    void onSourceConnected(const QString &sourceName, bool success);

private:
    void createSources();
    void createFormatters();
    void destroySources();
    void connectSourceAsync(const QString &sourceName);
    QString determineSeverity(const QString &type, double value);
    
    double getTotalRamMB();
    double getRamPercentage(double usedMB);

    QTimer *m_pollTimer;
    QTimer *m_reconnectTimer;
    
    std::atomic<bool> m_isRunning;
    std::atomic<bool> m_isStopping;
    mutable QMutex m_mutex;
    
    AppConfigData m_config;
    double m_totalRamMB;
    
    // Source entry - no atomics needed since map access is mutex-protected
    struct SourceEntry {
        std::unique_ptr<ITelemetrySource> source;
        QString telemetryType;
        int rateMs = 0;
        qint64 lastReadTime = 0;
        bool isConnected = false;
        bool isConnecting = false;
        int reconnectAttempts = 0;
        
        // Default constructor
        SourceEntry() = default;
        
        // Move constructor (needed for map insertion)
        SourceEntry(SourceEntry&& other) noexcept
            : source(std::move(other.source))
            , telemetryType(std::move(other.telemetryType))
            , rateMs(other.rateMs)
            , lastReadTime(other.lastReadTime)
            , isConnected(other.isConnected)
            , isConnecting(other.isConnecting)
            , reconnectAttempts(other.reconnectAttempts)
        {}
        
        // Move assignment (needed for map operations)
        SourceEntry& operator=(SourceEntry&& other) noexcept {
            if (this != &other) {
                source = std::move(other.source);
                telemetryType = std::move(other.telemetryType);
                rateMs = other.rateMs;
                lastReadTime = other.lastReadTime;
                isConnected = other.isConnected;
                isConnecting = other.isConnecting;
                reconnectAttempts = other.reconnectAttempts;
            }
            return *this;
        }
        
        // Delete copy operations
        SourceEntry(const SourceEntry&) = delete;
        SourceEntry& operator=(const SourceEntry&) = delete;
    };
    std::map<QString, SourceEntry> m_sources;
    
    std::map<QString, QFuture<bool>> m_pendingConnections;
    
    std::unique_ptr<LogFormatter<CpuPolicy>> m_cpuFormatter;
    std::unique_ptr<LogFormatter<GpuPolicy>> m_gpuFormatter;
    std::unique_ptr<LogFormatter<RamPolicy>> m_ramFormatter;
    
    static constexpr int MAX_RECONNECT_ATTEMPTS = 5;
    static constexpr int RECONNECT_INTERVAL_MS = 5000;
};

#endif // TELEMETRYAPPCONTROLLER_HPP