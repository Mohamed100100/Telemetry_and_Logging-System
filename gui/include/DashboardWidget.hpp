/**
 * @file DashboardWidget.hpp
 * @brief Dashboard showing real-time telemetry gauges
 */

#ifndef DASHBOARDWIDGET_HPP
#define DASHBOARDWIDGET_HPP

#include <QWidget>
#include <QMap>

class QGridLayout;
class QLabel;
class TelemetryGaugeWidget;

/**
 * @class DashboardWidget
 * @brief Displays real-time telemetry data using gauge widgets
 */
class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(QWidget *parent = nullptr);
    ~DashboardWidget();

public slots:
    void updateTelemetry(const QString &type, double value, const QString &severity);
    void clearAll();

private:
    void setupUI();
    QWidget* createGaugePanel(const QString &title, const QString &type, 
                              const QString &iconPath, const QString &unit,
                              double maxValue, double warningThreshold, 
                              double criticalThreshold);
    void updateStatistics(const QString &type, double value);

    QGridLayout *m_mainLayout;
    
    // Gauge widgets for each telemetry type
    QMap<QString, TelemetryGaugeWidget*> m_gauges;
    
    // Value labels
    QMap<QString, QLabel*> m_valueLabels;
    QMap<QString, QLabel*> m_statusLabels;
    
    // Statistics labels
    QMap<QString, QLabel*> m_minLabels;
    QMap<QString, QLabel*> m_maxLabels;
    QMap<QString, QLabel*> m_avgLabels;
    
    // Units and max values for each type
    QMap<QString, QString> m_units;
    QMap<QString, double> m_maxValues;
    
    // Statistics data
    struct Statistics {
        double min = 0.0;
        double max = 0.0;
        double sum = 0.0;
        int count = 0;
    };
    QMap<QString, Statistics> m_stats;
};

#endif // DASHBOARDWIDGET_HPP