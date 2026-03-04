/**
 * @file DashboardWidget.cpp
 * @brief Implementation of the dashboard widget
 */

#include "DashboardWidget.hpp"
#include "TelemetryGaugeWidget.hpp"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFrame>

// Include policies to get thresholds
#include "formatter/policies/CpuPolicy.hpp"
#include "formatter/policies/GpuPolicy.hpp"
#include "formatter/policies/RamPolicy.hpp"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

namespace {
    QString toQString(std::string_view sv) {
        return QString::fromUtf8(sv.data(), static_cast<int>(sv.size()));
    }
    
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

DashboardWidget::DashboardWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

DashboardWidget::~DashboardWidget()
{
}

void DashboardWidget::setupUI()
{
    m_mainLayout = new QGridLayout(this);
    m_mainLayout->setSpacing(20);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Get actual total RAM
    double totalRamMB = getTotalRamMB();
    qDebug() << "Dashboard: Total RAM detected:" << totalRamMB << "MB";
    
    // CPU panel
    QWidget *cpuPanel = createGaugePanel(
        "CPU Usage", "CPU", ":/icons/cpu.png", 
        toQString(CpuPolicy::unit),
        100.0,
        CpuPolicy::WARNING,
        CpuPolicy::CRITICAL
    );
    
    // GPU panel
    QWidget *gpuPanel = createGaugePanel(
        "GPU Usage", "GPU", ":/icons/gpu.png",
        toQString(GpuPolicy::unit),
        100.0,
        GpuPolicy::WARNING,
        GpuPolicy::CRITICAL
    );
    
    // RAM panel - use actual total RAM
    QWidget *ramPanel = createGaugePanel(
        "RAM Usage", "RAM", ":/icons/ram.png",
        toQString(RamPolicy::unit),
        totalRamMB,    // ← Use detected total RAM
        RamPolicy::WARNING,
        RamPolicy::CRITICAL
    );
    
    m_mainLayout->addWidget(cpuPanel, 0, 0);
    m_mainLayout->addWidget(gpuPanel, 0, 1);
    m_mainLayout->addWidget(ramPanel, 0, 2);
    
    m_mainLayout->setRowStretch(1, 1);
    
    setLayout(m_mainLayout);
}

QWidget* DashboardWidget::createGaugePanel(const QString &title, 
                                            const QString &type,
                                            const QString &iconPath,
                                            const QString &unit,
                                            double maxValue,
                                            double warningThreshold,
                                            double criticalThreshold)
{
    QGroupBox *groupBox = new QGroupBox(title, this);
    groupBox->setMinimumSize(300, 400);
    
    QVBoxLayout *layout = new QVBoxLayout(groupBox);
    layout->setSpacing(15);
    
    // Icon and title
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QLabel *iconLabel = new QLabel();
    iconLabel->setPixmap(QPixmap(iconPath).scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    headerLayout->addWidget(iconLabel);
    headerLayout->addStretch();
    layout->addLayout(headerLayout);
    
    // Gauge widget
    TelemetryGaugeWidget *gauge = new TelemetryGaugeWidget(this);
    gauge->setTitle(type);
    gauge->setUnit(unit);
    gauge->setMaxValue(maxValue);
    gauge->setWarningThreshold(warningThreshold);
    gauge->setCriticalThreshold(criticalThreshold);
    
    m_gauges[type] = gauge;
    m_maxValues[type] = maxValue;
    m_units[type] = unit;
    layout->addWidget(gauge, 1);
    
    // Current value label
    QLabel *valueLabel = new QLabel(QString("-- %1").arg(unit));
    valueLabel->setAlignment(Qt::AlignCenter);
    valueLabel->setStyleSheet("font-size: 24px; font-weight: bold;");
    m_valueLabels[type] = valueLabel;
    layout->addWidget(valueLabel);
    
    // Status label
    QLabel *statusLabel = new QLabel("No Data");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("font-size: 14px; color: #888;");
    m_statusLabels[type] = statusLabel;
    layout->addWidget(statusLabel);
    
    // Statistics frame
    QFrame *statsFrame = new QFrame();
    statsFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    QGridLayout *statsLayout = new QGridLayout(statsFrame);
    
    statsLayout->addWidget(new QLabel("Min:"), 0, 0);
    QLabel *minLabel = new QLabel(QString("-- %1").arg(unit));
    minLabel->setAlignment(Qt::AlignRight);
    m_minLabels[type] = minLabel;
    statsLayout->addWidget(minLabel, 0, 1);
    
    statsLayout->addWidget(new QLabel("Max:"), 1, 0);
    QLabel *maxLabel = new QLabel(QString("-- %1").arg(unit));
    maxLabel->setAlignment(Qt::AlignRight);
    m_maxLabels[type] = maxLabel;
    statsLayout->addWidget(maxLabel, 1, 1);
    
    statsLayout->addWidget(new QLabel("Avg:"), 2, 0);
    QLabel *avgLabel = new QLabel(QString("-- %1").arg(unit));
    avgLabel->setAlignment(Qt::AlignRight);
    m_avgLabels[type] = avgLabel;
    statsLayout->addWidget(avgLabel, 2, 1);
    
    layout->addWidget(statsFrame);
    
    // Initialize statistics
    m_stats[type] = Statistics();
    
    return groupBox;
}

void DashboardWidget::updateTelemetry(const QString &type, double value, 
                                       const QString &severity)
{
    if (!m_gauges.contains(type)) {
        return;
    }
    
    QString unit = m_units.value(type, "%");
    double maxValue = m_maxValues.value(type, 100.0);
    
    // For RAM, auto-adjust max value if needed
    if (type == "RAM" && value > maxValue * 0.9) {
        // Round up to nearest 8GB (8192 MB)
        double newMax = std::ceil(value / 8192.0) * 8192.0;
        m_maxValues[type] = newMax;
        maxValue = newMax;
        m_gauges[type]->setMaxValue(newMax);
    }
    
    // Calculate gauge value (percentage for gauge display)
    double gaugeValue;
    if (type == "RAM") {
        // RAM value is in MB, convert to percentage for gauge
        gaugeValue = (value / maxValue) * 100.0;
    } else {
        // CPU/GPU already in percentage
        gaugeValue = value;
    }
    
    // Update gauge
    m_gauges[type]->setValue(gaugeValue);
    m_gauges[type]->setSeverity(severity);
    
    // Update value label with actual value and unit
    if (type == "RAM") {
        // Display RAM in MB (or GB if large enough)
        if (value >= 1024.0) {
            m_valueLabels[type]->setText(QString("%1 GB").arg(value / 1024.0, 0, 'f', 2));
        } else {
            m_valueLabels[type]->setText(QString("%1 MB").arg(value, 0, 'f', 0));
        }
    } else {
        m_valueLabels[type]->setText(QString("%1%2").arg(value, 0, 'f', 2).arg(unit));
    }
    
    // Update status label
    QString statusText;
    QString statusColor;
    
    if (severity == "CRITICAL") {
        statusText = "CRITICAL";
        statusColor = "#e74c3c";
    } else if (severity == "WARNING") {
        statusText = "Warning";
        statusColor = "#f39c12";
    } else {
        statusText = "Normal";
        statusColor = "#27ae60";
    }
    
    m_statusLabels[type]->setText(statusText);
    m_statusLabels[type]->setStyleSheet(
        QString("font-size: 14px; font-weight: bold; color: %1;").arg(statusColor));
    
    // Update statistics
    updateStatistics(type, value);
}

void DashboardWidget::updateStatistics(const QString &type, double value)
{
    Statistics &stats = m_stats[type];
    QString unit = m_units.value(type, "%");
    
    if (stats.count == 0 || value < stats.min) stats.min = value;
    if (stats.count == 0 || value > stats.max) stats.max = value;
    stats.sum += value;
    stats.count++;
    
    double avg = stats.sum / stats.count;
    
    // Format based on type
    if (type == "RAM") {
        // Format RAM statistics - show in GB if large enough
        auto formatRam = [](double val) -> QString {
            if (val >= 1024.0) {
                return QString("%1 GB").arg(val / 1024.0, 0, 'f', 2);
            }
            return QString("%1 MB").arg(val, 0, 'f', 0);
        };
        
        m_minLabels[type]->setText(formatRam(stats.min));
        m_maxLabels[type]->setText(formatRam(stats.max));
        m_avgLabels[type]->setText(formatRam(avg));
    } else {
        m_minLabels[type]->setText(QString("%1%2").arg(stats.min, 0, 'f', 2).arg(unit));
        m_maxLabels[type]->setText(QString("%1%2").arg(stats.max, 0, 'f', 2).arg(unit));
        m_avgLabels[type]->setText(QString("%1%2").arg(avg, 0, 'f', 2).arg(unit));
    }
}

void DashboardWidget::clearAll()
{
    for (const QString &type : m_gauges.keys()) {
        QString unit = m_units.value(type, "%");
        m_gauges[type]->setValue(0);
        m_valueLabels[type]->setText(QString("-- %1").arg(unit));
        m_statusLabels[type]->setText("No Data");
        m_statusLabels[type]->setStyleSheet("font-size: 14px; color: #888;");
        m_minLabels[type]->setText(QString("-- %1").arg(unit));
        m_maxLabels[type]->setText(QString("-- %1").arg(unit));
        m_avgLabels[type]->setText(QString("-- %1").arg(unit));
        m_stats[type] = Statistics();
    }
}