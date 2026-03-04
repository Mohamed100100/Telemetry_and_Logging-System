/**
 * @file TelemetryGaugeWidget.cpp
 * @brief Implementation of the telemetry gauge widget
 */

#include "TelemetryGaugeWidget.hpp"

#include <QPainter>
#include <QPainterPath>
#include <QTimer>
#include <QtMath>

TelemetryGaugeWidget::TelemetryGaugeWidget(QWidget *parent)
    : QWidget(parent)
    , m_value(0.0)
    , m_targetValue(0.0)
    , m_warningThreshold(75.0)
    , m_criticalThreshold(90.0)
    , m_maxValue(100.0)
    , m_unit("%")
    , m_title("")
    , m_severity("INFO")
    , m_backgroundColor(QColor(45, 45, 45))
    , m_normalColor(QColor(39, 174, 96))
    , m_warningColor(QColor(243, 156, 18))
    , m_criticalColor(QColor(231, 76, 60))
    , m_textColor(QColor(255, 255, 255))
{
    setMinimumSize(150, 150);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // Animation timer
    m_animationTimer = new QTimer(this);
    m_animationTimer->setInterval(16); // ~60 FPS
    connect(m_animationTimer, &QTimer::timeout, this, [this]() {
        if (qAbs(m_value - m_targetValue) > 0.1) {
            m_value += (m_targetValue - m_value) * 0.15;
            update();
        } else {
            m_value = m_targetValue;
            m_animationTimer->stop();
            update();
        }
    });
}

TelemetryGaugeWidget::~TelemetryGaugeWidget()
{
}

double TelemetryGaugeWidget::value() const
{
    return m_targetValue;
}

double TelemetryGaugeWidget::warningThreshold() const
{
    return m_warningThreshold;
}

double TelemetryGaugeWidget::criticalThreshold() const
{
    return m_criticalThreshold;
}

double TelemetryGaugeWidget::maxValue() const
{
    return m_maxValue;
}

void TelemetryGaugeWidget::setValue(double value)
{
    // Value is expected as percentage (0-100)
    value = qBound(0.0, value, 100.0);
    
    if (!qFuzzyCompare(m_targetValue, value)) {
        m_targetValue = value;
        m_animationTimer->start();
        emit valueChanged(value);
    }
}

void TelemetryGaugeWidget::setSeverity(const QString &severity)
{
    m_severity = severity;
    update();
}

void TelemetryGaugeWidget::setWarningThreshold(double threshold)
{
    m_warningThreshold = threshold;
    update();
}

void TelemetryGaugeWidget::setCriticalThreshold(double threshold)
{
    m_criticalThreshold = threshold;
    update();
}

void TelemetryGaugeWidget::setUnit(const QString &unit)
{
    m_unit = unit;
    update();
}

void TelemetryGaugeWidget::setTitle(const QString &title)
{
    m_title = title;
    update();
}

void TelemetryGaugeWidget::setMaxValue(double maxValue)
{
    m_maxValue = maxValue;
    update();
}

QSize TelemetryGaugeWidget::sizeHint() const
{
    return QSize(200, 200);
}

QSize TelemetryGaugeWidget::minimumSizeHint() const
{
    return QSize(150, 150);
}

void TelemetryGaugeWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    drawBackground(painter);
    drawArc(painter);
    drawValue(painter);
    drawTitle(painter);
}

void TelemetryGaugeWidget::drawBackground(QPainter &painter)
{
    int side = qMin(width(), height());
    QRectF rect((width() - side) / 2.0, (height() - side) / 2.0, side, side);
    rect.adjust(10, 10, -10, -10);
    
    // Draw background circle
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_backgroundColor);
    painter.drawEllipse(rect);
    
    // Draw track arc
    int arcWidth = side / 10;
    rect.adjust(arcWidth, arcWidth, -arcWidth, -arcWidth);
    
    QPen trackPen(QColor(80, 80, 80), arcWidth, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(trackPen);
    painter.setBrush(Qt::NoBrush);
    
    // Arc spans from 225 degrees to -45 degrees (270 degree span)
    painter.drawArc(rect, 225 * 16, -270 * 16);
}

void TelemetryGaugeWidget::drawArc(QPainter &painter)
{
    int side = qMin(width(), height());
    QRectF rect((width() - side) / 2.0, (height() - side) / 2.0, side, side);
    
    int arcWidth = side / 10;
    rect.adjust(10 + arcWidth, 10 + arcWidth, -10 - arcWidth, -10 - arcWidth);
    
    // Calculate arc span based on value (value is percentage 0-100)
    double normalizedValue = m_value / 100.0;
    int spanAngle = static_cast<int>(-270 * normalizedValue * 16);
    
    // Get color based on current severity
    QColor arcColor = getValueColor();
    
    QPen arcPen(arcColor, arcWidth, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(arcPen);
    painter.setBrush(Qt::NoBrush);
    
    painter.drawArc(rect, 225 * 16, spanAngle);
}

void TelemetryGaugeWidget::drawValue(QPainter &painter)
{
    QFont font = painter.font();
    font.setPointSize(qMin(width(), height()) / 8);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(m_textColor);
    
    // Display percentage value
    QString text = QString("%1%").arg(m_value, 0, 'f', 0);
    
    QRectF textRect = rect();
    textRect.moveTop(textRect.top() + textRect.height() * 0.1);
    painter.drawText(textRect, Qt::AlignCenter, text);
}

void TelemetryGaugeWidget::drawTitle(QPainter &painter)
{
    if (m_title.isEmpty()) return;
    
    QFont font = painter.font();
    font.setPointSize(qMin(width(), height()) / 12);
    font.setBold(false);
    painter.setFont(font);
    painter.setPen(QColor(180, 180, 180));
    
    QRectF textRect = rect();
    textRect.moveTop(textRect.top() + textRect.height() * 0.65);
    painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, m_title);
}

QColor TelemetryGaugeWidget::getValueColor() const
{
    if (m_severity == "CRITICAL" || m_value >= m_criticalThreshold) {
        return m_criticalColor;
    } else if (m_severity == "WARNING" || m_value >= m_warningThreshold) {
        return m_warningColor;
    }
    return m_normalColor;
}