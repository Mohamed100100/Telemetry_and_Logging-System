/**
 * @file TelemetryAppController.cpp
 * @brief Implementation of the telemetry application controller
 */

#include "TelemetryAppController.hpp"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QDateTime>
#include <QElapsedTimer>
#include <QTextStream>
#include <QRegularExpression>
#include <QtConcurrent/QtConcurrent>

#include "sources/FileTelemetrySourceImpl.hpp"
#include "sources/SocketTelemetrySourceImpl.hpp"

#ifdef SOMEIP_ENABLED
#include "sources/SomeIPTelemetrySourceAdapter.hpp"
#endif

#include "formatter/LogFormatter.hpp"
#include "formatter/policies/CpuPolicy.hpp"
#include "formatter/policies/GpuPolicy.hpp"
#include "formatter/policies/RamPolicy.hpp"
#include "logger/LogMessage.hpp"

namespace {
    QString toQString(std::string_view sv) {
        return QString::fromUtf8(sv.data(), static_cast<int>(sv.size()));
    }
}

TelemetryAppController::TelemetryAppController(QObject *parent)
    : QObject(parent)
    , m_pollTimer(new QTimer(this))
    , m_reconnectTimer(new QTimer(this))
    , m_isRunning(false)
    , m_isStopping(false)
    , m_totalRamMB(0.0)
{
    m_config.appName = "TelemetryApp";
    m_config.bufferSize = 100;
    m_config.threadPoolSize = 4;
    
    m_totalRamMB = getTotalRamMB();
    qDebug() << "Total RAM detected:" << m_totalRamMB << "MB (" << (m_totalRamMB / 1024.0) << "GB)";
    
    connect(m_pollTimer, &QTimer::timeout, this, &TelemetryAppController::pollTelemetry);
    
    connect(m_reconnectTimer, &QTimer::timeout, this, [this]() {
        if (!m_isRunning.load() || m_isStopping.load()) {
            return;
        }
        
        QMutexLocker locker(&m_mutex);
        for (auto &[sourceName, entry] : m_sources) {
            if (!entry.isConnected && 
                !entry.isConnecting && 
                entry.reconnectAttempts < MAX_RECONNECT_ATTEMPTS) {
                QString name = sourceName;  // Copy for use after unlock
                locker.unlock();
                connectSourceAsync(name);
                locker.relock();
            }
        }
    });
}

TelemetryAppController::~TelemetryAppController()
{
    stop();
    
    for (auto &[name, future] : m_pendingConnections) {
        if (future.isRunning()) {
            future.waitForFinished();
        }
    }
}

//=============================================================================
// RAM Helper Functions
//=============================================================================

double TelemetryAppController::getTotalRamMB()
{
    QFile file("/proc/meminfo");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("MemTotal:")) {
                QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
                if (parts.size() >= 2) {
                    double totalKB = parts[1].toDouble();
                    file.close();
                    return totalKB / 1024.0;
                }
            }
        }
        file.close();
    }
    
    qWarning() << "Could not read /proc/meminfo, assuming 16GB RAM";
    return 16384.0;
}

double TelemetryAppController::getRamPercentage(double usedMB)
{
    if (m_totalRamMB <= 0.0) {
        return 0.0;
    }
    return (usedMB / m_totalRamMB) * 100.0;
}

//=============================================================================
// Public Methods
//=============================================================================

bool TelemetryAppController::isRunning() const
{
    return m_isRunning.load();
}

AppConfigData TelemetryAppController::getConfig() const
{
    QMutexLocker locker(&m_mutex);
    return m_config;
}

void TelemetryAppController::setConfig(const AppConfigData &config)
{
    QMutexLocker locker(&m_mutex);
    m_config = config;
}

bool TelemetryAppController::loadConfig(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        emit errorOccurred(QString("Failed to open config file: %1").arg(filePath));
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (doc.isNull()) {
        emit errorOccurred("Invalid JSON configuration file");
        return false;
    }
    
    try {
        QJsonObject root = doc.object();
        
        QMutexLocker locker(&m_mutex);
        
        QJsonObject appObj = root["application"].toObject();
        m_config.appName = appObj["name"].toString("TelemetryApp");
        
        m_config.sources.clear();
        QJsonArray sourcesArray = root["sources"].toArray();
        for (const QJsonValue &val : sourcesArray) {
            QJsonObject obj = val.toObject();
            SourceConfigData source;
            source.type = obj["type"].toString();
            source.path = obj["path"].toString();
            source.telemetryType = obj["telemetryType"].toString();
            source.rateMs = obj["rateMs"].toInt(500);
            m_config.sources.append(source);
        }
        
        m_config.sinks.clear();
        QJsonArray sinksArray = root["sinks"].toArray();
        for (const QJsonValue &val : sinksArray) {
            QJsonObject obj = val.toObject();
            SinkConfigData sink;
            sink.type = obj["type"].toString();
            sink.path = obj["path"].toString();
            m_config.sinks.append(sink);
        }
        
        return true;
        
    } catch (const std::exception &e) {
        emit errorOccurred(QString("Error parsing config: %1").arg(e.what()));
        return false;
    }
}

bool TelemetryAppController::saveConfig(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);
    
    QJsonObject root;
    
    QJsonObject appObj;
    appObj["name"] = m_config.appName;
    root["application"] = appObj;
    
    QJsonArray sourcesArray;
    for (const SourceConfigData &source : m_config.sources) {
        QJsonObject obj;
        obj["type"] = source.type;
        obj["path"] = source.path;
        obj["telemetryType"] = source.telemetryType;
        obj["rateMs"] = source.rateMs;
        sourcesArray.append(obj);
    }
    root["sources"] = sourcesArray;
    
    QJsonArray sinksArray;
    for (const SinkConfigData &sink : m_config.sinks) {
        QJsonObject obj;
        obj["type"] = sink.type;
        obj["path"] = sink.path;
        sinksArray.append(obj);
    }
    root["sinks"] = sinksArray;
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        emit errorOccurred(QString("Failed to save config file: %1").arg(filePath));
        return false;
    }
    
    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

//=============================================================================
// Private Methods
//=============================================================================

void TelemetryAppController::createFormatters()
{
    std::string appName = m_config.appName.toStdString();
    m_cpuFormatter = std::make_unique<LogFormatter<CpuPolicy>>(appName);
    m_gpuFormatter = std::make_unique<LogFormatter<GpuPolicy>>(appName);
    m_ramFormatter = std::make_unique<LogFormatter<RamPolicy>>(appName);
}

void TelemetryAppController::createSources()
{
    m_sources.clear();
    
    for (const SourceConfigData &srcConfig : m_config.sources) {
        QString sourceName = srcConfig.telemetryType + " (" + srcConfig.type + ")";
        
        SourceEntry entry;
        entry.telemetryType = srcConfig.telemetryType;
        entry.rateMs = srcConfig.rateMs;
        entry.lastReadTime = 0;
        entry.isConnected = false;
        entry.isConnecting = false;
        entry.reconnectAttempts = 0;
        
        try {
            if (srcConfig.type == "file") {
                std::string filePath = srcConfig.path.toStdString();
                entry.source = std::make_unique<FileTelemetrySourceImpl>(filePath);
            } 
            else if (srcConfig.type == "socket") {
                std::string socketPath = srcConfig.path.toStdString();
                entry.source = std::make_unique<SocketTelemetrySourceImpl>(socketPath);
            }
#ifdef SOMEIP_ENABLED
            else if (srcConfig.type == "someip") {
                entry.source = std::make_unique<SomeIPTelemetrySourceAdapter>();
            }
#endif
            else {
                qWarning() << "Unknown source type:" << srcConfig.type;
                continue;
            }
            
            m_sources.emplace(sourceName, std::move(entry));
            
        } catch (const std::exception &e) {
            qWarning() << "Failed to create source" << sourceName << ":" << e.what();
            emit errorOccurred(QString("Failed to create source %1: %2")
                              .arg(sourceName).arg(e.what()));
        }
    }
}

void TelemetryAppController::destroySources()
{
    m_pendingConnections.clear();
    m_sources.clear();
}

void TelemetryAppController::connectSourceAsync(const QString &sourceName)
{
    if (m_isStopping.load()) {
        return;
    }
    
    QMutexLocker locker(&m_mutex);
    
    auto it = m_sources.find(sourceName);
    if (it == m_sources.end() || !it->second.source) {
        return;
    }
    
    // Mark as connecting to prevent duplicate attempts
    if (it->second.isConnecting) {
        return;  // Already connecting
    }
    it->second.isConnecting = true;
    
    // Get raw pointer for the lambda
    ITelemetrySource* sourcePtr = it->second.source.get();
    
    locker.unlock();
    
    // Run connection in background thread
    QFuture<bool> future = QtConcurrent::run([sourcePtr]() -> bool {
        return sourcePtr->openSource();
    });
    
    m_pendingConnections[sourceName] = future;
    
    // Use QFutureWatcher to get notified when done
    auto *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher, sourceName]() {
        if (!m_isStopping.load()) {
            bool success = watcher->result();
            onSourceConnected(sourceName, success);
        }
        watcher->deleteLater();
        m_pendingConnections.erase(sourceName);
    });
    
    watcher->setFuture(future);
}

void TelemetryAppController::onSourceConnected(const QString &sourceName, bool success)
{
    if (m_isStopping.load()) {
        return;
    }
    
    QMutexLocker locker(&m_mutex);
    
    auto it = m_sources.find(sourceName);
    if (it == m_sources.end()) {
        return;
    }
    
    it->second.isConnecting = false;
    it->second.isConnected = success;
    
    if (success) {
        it->second.reconnectAttempts = 0;
        qDebug() << "Source connected:" << sourceName;
    } else {
        it->second.reconnectAttempts++;
        qDebug() << "Source connection failed:" << sourceName 
                 << "attempt:" << it->second.reconnectAttempts;
    }
    
    locker.unlock();
    
    emit sourceStatusChanged(sourceName, success);
}

void TelemetryAppController::start()
{
    if (m_isRunning.load()) {
        return;
    }
    
    m_isStopping.store(false);
    
    QMutexLocker locker(&m_mutex);
    
    if (m_config.sources.isEmpty()) {
        locker.unlock();
        emit errorOccurred("No sources configured");
        return;
    }
    
    try {
        createFormatters();
        createSources();
        
        m_isRunning.store(true);
        
        // Get list of source names before unlocking
        QStringList sourceNames;
        for (const auto &[name, entry] : m_sources) {
            sourceNames.append(name);
        }
        
        locker.unlock();
        
        // Start async connection for all sources
        for (const QString &sourceName : sourceNames) {
            connectSourceAsync(sourceName);
        }
        
        // Start polling timer
        int minRate = 100;
        for (const auto &src : m_config.sources) {
            if (src.rateMs < minRate) {
                minRate = src.rateMs;
            }
        }
        m_pollTimer->start(minRate / 2);
        
        // Start reconnection timer
        m_reconnectTimer->start(RECONNECT_INTERVAL_MS);
        
        emit started();
        
    } catch (const std::exception &e) {
        emit errorOccurred(QString("Failed to start: %1").arg(e.what()));
        m_isRunning.store(false);
    }
}

void TelemetryAppController::stop()
{
    if (!m_isRunning.load()) {
        return;
    }
    
    m_isStopping.store(true);
    
    m_pollTimer->stop();
    m_reconnectTimer->stop();
    m_isRunning.store(false);
    
    {
        QMutexLocker locker(&m_mutex);
        
        QStringList sourceNames;
        for (const auto &[sourceName, entry] : m_sources) {
            sourceNames.append(sourceName);
        }
        
        locker.unlock();
        
        for (const QString &name : sourceNames) {
            emit sourceStatusChanged(name, false);
        }
        
        locker.relock();
        
        destroySources();
        
        m_cpuFormatter.reset();
        m_gpuFormatter.reset();
        m_ramFormatter.reset();
    }
    
    emit stopped();
}

void TelemetryAppController::restart()
{
    stop();
    start();
}

QString TelemetryAppController::determineSeverity(const QString &type, double value)
{
    if (type == "CPU") {
        if (value >= CpuPolicy::CRITICAL) return "CRITICAL";
        if (value >= CpuPolicy::WARNING) return "WARNING";
    } else if (type == "GPU") {
        if (value >= GpuPolicy::CRITICAL) return "CRITICAL";
        if (value >= GpuPolicy::WARNING) return "WARNING";
    } else if (type == "RAM") {
        double ramPercentage = getRamPercentage(value);
        if (ramPercentage >= RamPolicy::CRITICAL) return "CRITICAL";
        if (ramPercentage >= RamPolicy::WARNING) return "WARNING";
    }
    return "INFO";
}

void TelemetryAppController::pollTelemetry()
{
    if (!m_isRunning.load() || m_isStopping.load()) {
        return;
    }
    
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    
    // Try to lock without blocking
    if (!m_mutex.tryLock()) {
        return;
    }
    
    // Collect data for sources that need polling
    struct PollData {
        QString sourceName;
        QString telemetryType;
        int rateMs;
        qint64 lastReadTime;
        ITelemetrySource* source;
    };
    
    QVector<PollData> toPoll;
    
    for (auto &[sourceName, entry] : m_sources) {
        // Skip if not connected or currently connecting
        if (!entry.isConnected || entry.isConnecting) {
            continue;
        }
        
        // Check if it's time to read from this source
        if (currentTime - entry.lastReadTime < entry.rateMs) {
            continue;
        }
        
        if (!entry.source) {
            continue;
        }
        
        PollData pd;
        pd.sourceName = sourceName;
        pd.telemetryType = entry.telemetryType;
        pd.rateMs = entry.rateMs;
        pd.lastReadTime = entry.lastReadTime;
        pd.source = entry.source.get();
        toPoll.append(pd);
    }
    
    m_mutex.unlock();
    
    // Process outside the lock
    for (const PollData &pd : toPoll) {
        if (m_isStopping.load()) {
            return;
        }
        
        std::string data;
        bool readSuccess = pd.source->readSource(data);
        
        if (readSuccess && !data.empty()) {
            // Update last read time
            {
                QMutexLocker locker(&m_mutex);
                auto it = m_sources.find(pd.sourceName);
                if (it != m_sources.end()) {
                    it->second.lastReadTime = currentTime;
                }
            }
            
            double value = 0.0;
            try {
                value = std::stod(data);
            } catch (...) {
                qWarning() << "Failed to parse telemetry data:" << QString::fromStdString(data);
                continue;
            }
            
            QString type = pd.telemetryType;
            QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            QString message;
            QString severity;
            
            std::optional<LogMessage> logMsg;
            
            if (type == "CPU" && m_cpuFormatter) {
                logMsg = m_cpuFormatter->formatDataToLogMsg(data);
            } else if (type == "GPU" && m_gpuFormatter) {
                logMsg = m_gpuFormatter->formatDataToLogMsg(data);
            } else if (type == "RAM" && m_ramFormatter) {
                double ramPercentage = getRamPercentage(value);
                std::string percentStr = std::to_string(ramPercentage);
                logMsg = m_ramFormatter->formatDataToLogMsg(percentStr);
            }
            
            severity = determineSeverity(type, value);
            
            if (logMsg.has_value()) {
                message = QString::fromStdString(logMsg.value().ToString());
            } else {
                qWarning() << "Formatter returned nullopt for" << type << "value:" << value;
                
                if (type == "CPU") {
                    message = QString("CPU usage: %1%2").arg(value, 0, 'f', 2).arg(toQString(CpuPolicy::unit));
                } else if (type == "GPU") {
                    message = QString("GPU usage: %1%2").arg(value, 0, 'f', 2).arg(toQString(GpuPolicy::unit));
                } else if (type == "RAM") {
                    double ramPercentage = getRamPercentage(value);
                    message = QString("RAM usage: %1 MB (%2%)")
                        .arg(value, 0, 'f', 0)
                        .arg(ramPercentage, 0, 'f', 1);
                }
            }
            
            emit telemetryReceived(type, value, severity);
            emit logReceived(timestamp, severity, type, message);
            
        } else {
            // Read failed - mark source as disconnected
            QMutexLocker locker(&m_mutex);
            auto it = m_sources.find(pd.sourceName);
            if (it != m_sources.end() && it->second.isConnected) {
                it->second.isConnected = false;
                locker.unlock();
                emit sourceStatusChanged(pd.sourceName, false);
                qDebug() << "Source disconnected:" << pd.sourceName;
            }
            // Reconnection will be handled by m_reconnectTimer
        }
    }
}