/**
 * @file TelemetryChartWidget.cpp
 * @brief Implementation of the real-time telemetry chart widget
 */

#include "TelemetryChartWidget.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <cmath>

// Helper to get total RAM
namespace {
    double getTotalRamMB() {
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
        return 16384.0; // Fallback: 16 GB
    }
}

TelemetryChartWidget::TelemetryChartWidget(QWidget *parent)
    : QWidget(parent)
    , m_timeWindowSeconds(60)
    , m_maxDataPoints(300)
    , m_totalRamMB(getTotalRamMB())  // Cache total RAM
{
    // Initialize colors
    m_seriesColors["CPU"] = QColor(52, 152, 219);   // Blue
    m_seriesColors["GPU"] = QColor(155, 89, 182);   // Purple
    m_seriesColors["RAM"] = QColor(46, 204, 113);   // Green
    
    setupChart();
}

TelemetryChartWidget::~TelemetryChartWidget()
{
}

void TelemetryChartWidget::setupChart()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    
    // Create chart
    m_chart = new QChart();
    m_chart->setTitle("Telemetry History (%)");
    m_chart->setAnimationOptions(QChart::NoAnimation);
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignBottom);
    
    // Set chart theme colors for dark mode
    m_chart->setBackgroundBrush(QBrush(QColor(45, 45, 45)));
    m_chart->setTitleBrush(QBrush(Qt::white));
    m_chart->legend()->setLabelColor(Qt::white);
    
    // Create axes
    m_axisX = new QDateTimeAxis();
    m_axisX->setFormat("hh:mm:ss");
    m_axisX->setTitleText("Time");
    m_axisX->setLabelsColor(Qt::white);
    m_axisX->setTitleBrush(QBrush(Qt::white));
    m_axisX->setGridLineColor(QColor(80, 80, 80));
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    
    m_axisY = new QValueAxis();
    m_axisY->setRange(0, 100);
    m_axisY->setTitleText("Usage (%)");
    m_axisY->setLabelsColor(Qt::white);
    m_axisY->setTitleBrush(QBrush(Qt::white));
    m_axisY->setGridLineColor(QColor(80, 80, 80));
    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    
    // Create series for each telemetry type
    QStringList types = {"CPU", "GPU", "RAM"};
    for (const QString &type : types) {
        QLineSeries *series = new QLineSeries();
        series->setName(type);
        series->setColor(m_seriesColors[type]);
        
        QPen pen = series->pen();
        pen.setWidth(2);
        series->setPen(pen);
        
        m_chart->addSeries(series);
        series->attachAxis(m_axisX);
        series->attachAxis(m_axisY);
        
        m_series[type] = series;
        m_dataPoints[type] = std::deque<DataPoint>();
    }
    
    // Create chart view
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setBackgroundBrush(QBrush(QColor(30, 30, 30)));
    
    layout->addWidget(m_chartView);
    
    // Initialize axis range
    QDateTime now = QDateTime::currentDateTime();
    m_axisX->setRange(now.addSecs(-m_timeWindowSeconds), now);
}

void TelemetryChartWidget::addDataPoint(const QString &type, double value)
{
    if (!m_series.contains(type)) {
        return;
    }
    
    QDateTime now = QDateTime::currentDateTime();
    
    // Convert value to percentage for chart display
    double percentageValue = value;
    
    if (type == "RAM") {
        // RAM value is in MB, convert to percentage
        if (m_totalRamMB > 0) {
            percentageValue = (value / m_totalRamMB) * 100.0;
        }
    }
    // CPU and GPU are already in percentage
    
    // Clamp to 0-100 range
    percentageValue = qBound(0.0, percentageValue, 100.0);
    
    // Add to data storage
    DataPoint dp;
    dp.timestamp = now;
    dp.value = percentageValue;
    m_dataPoints[type].push_back(dp);
    
    // Add to series
    m_series[type]->append(now.toMSecsSinceEpoch(), percentageValue);
    
    // Remove old data
    removeOldData();
    
    // Update axis ranges
    updateAxisRanges();
}

void TelemetryChartWidget::clearChart()
{
    for (const QString &type : m_series.keys()) {
        m_series[type]->clear();
        m_dataPoints[type].clear();
    }
    
    QDateTime now = QDateTime::currentDateTime();
    m_axisX->setRange(now.addSecs(-m_timeWindowSeconds), now);
    m_axisY->setRange(0, 100);
}

void TelemetryChartWidget::setTimeWindow(int seconds)
{
    m_timeWindowSeconds = seconds;
    removeOldData();
    updateAxisRanges();
}

void TelemetryChartWidget::setSeriesVisible(const QString &type, bool visible)
{
    if (m_series.contains(type)) {
        m_series[type]->setVisible(visible);
    }
}

void TelemetryChartWidget::updateAxisRanges()
{
    QDateTime now = QDateTime::currentDateTime();
    m_axisX->setRange(now.addSecs(-m_timeWindowSeconds), now);
    // Y axis is always 0-100% for all telemetry types
    m_axisY->setRange(0, 100);
}

void TelemetryChartWidget::removeOldData()
{
    QDateTime cutoff = QDateTime::currentDateTime().addSecs(-m_timeWindowSeconds);
    qint64 cutoffMs = cutoff.toMSecsSinceEpoch();
    
    for (const QString &type : m_dataPoints.keys()) {
        // Remove from data storage
        while (!m_dataPoints[type].empty() && 
               m_dataPoints[type].front().timestamp < cutoff) {
            m_dataPoints[type].pop_front();
        }
        
        // Remove from series
        QLineSeries *series = m_series[type];
        QList<QPointF> points = series->points();
        
        while (!points.isEmpty() && points.first().x() < cutoffMs) {
            points.removeFirst();
        }
        
        series->replace(points);
    }
}