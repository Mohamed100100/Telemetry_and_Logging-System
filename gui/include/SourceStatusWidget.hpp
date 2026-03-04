/**
 * @file SourceStatusWidget.hpp
 * @brief Widget showing status of telemetry sources
 */

#ifndef SOURCESTATUSWIDGET_HPP
#define SOURCESTATUSWIDGET_HPP

#include <QWidget>
#include <QMap>
#include <QDateTime>  

class QVBoxLayout;
class QLabel;
class QFrame;

/**
 * @class SourceStatusWidget
 * @brief Displays the connection status of all telemetry sources
 * 
 * Shows:
 * - Source name and type
 * - Connection status (connected/disconnected)
 * - Last received data timestamp
 * - Data rate
 */
class SourceStatusWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SourceStatusWidget(QWidget *parent = nullptr);
    ~SourceStatusWidget();

public slots:
    /**
     * @brief Add a source to monitor
     * @param name Source name
     * @param type Source type (file, socket, someip)
     */
    void addSource(const QString &name, const QString &type);
    
    /**
     * @brief Remove a source from monitoring
     * @param name Source name
     */
    void removeSource(const QString &name);
    
    /**
     * @brief Update source connection status
     * @param name Source name
     * @param isConnected Whether the source is connected
     */
    void setSourceStatus(const QString &name, bool isConnected);
    
    /**
     * @brief Update last data received time
     * @param name Source name
     */
    void updateLastDataTime(const QString &name);
    
    /**
     * @brief Clear all sources
     */
    void clearSources();

private:
    void setupUI();
    QFrame* createSourceCard(const QString &name, const QString &type);
    void updateSourceCard(const QString &name);

    QVBoxLayout *m_mainLayout;
    
    struct SourceInfo {
        QString type;
        bool isConnected;
        QDateTime lastDataTime;
        int dataCount;
        QFrame *card;
        QLabel *statusLabel;
        QLabel *lastDataLabel;
        QLabel *rateLabel;
    };
    
    QMap<QString, SourceInfo> m_sources;
};

#endif // SOURCESTATUSWIDGET_HPP
