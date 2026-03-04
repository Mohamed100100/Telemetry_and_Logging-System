/**
 * @file TelemetryChartWidget.hpp
 * @brief Real-time chart widget for telemetry data visualization
 */

#ifndef TELEMETRYCHARTWIDGET_HPP
#define TELEMETRYCHARTWIDGET_HPP

#include <QWidget>
#include <QMap>
#include <QDateTime>
#include <deque>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#else
#include <QtCharts>
#endif

/**
 * @class TelemetryChartWidget
 * @brief Displays real-time telemetry data as line charts
 * 
 * All values are displayed as percentages (0-100%)
 * - CPU: Direct percentage
 * - GPU: Direct percentage  
 * - RAM: Converted from MB to percentage
 */
class TelemetryChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TelemetryChartWidget(QWidget *parent = nullptr);
    ~TelemetryChartWidget();

public slots:
    /**
     * @brief Add a data point to the chart
     * @param type Telemetry type (CPU, GPU, RAM)
     * @param value Value (percentage for CPU/GPU, MB for RAM - will be converted)
     */
    void addDataPoint(const QString &type, double value);
    
    void clearChart();
    void setTimeWindow(int seconds);
    void setSeriesVisible(const QString &type, bool visible);

private:
    void setupChart();
    void updateAxisRanges();
    void removeOldData();

    QChartView *m_chartView;
    QChart *m_chart;
    
    QMap<QString, QLineSeries*> m_series;
    
    QDateTimeAxis *m_axisX;
    QValueAxis *m_axisY;
    
    struct DataPoint {
        QDateTime timestamp;
        double value;
    };
    QMap<QString, std::deque<DataPoint>> m_dataPoints;
    
    int m_timeWindowSeconds;
    int m_maxDataPoints;
    
    QMap<QString, QColor> m_seriesColors;
    
    // Total RAM for percentage calculation
    double m_totalRamMB;
};

#endif // TELEMETRYCHARTWIDGET_HPP