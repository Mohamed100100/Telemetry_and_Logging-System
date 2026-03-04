/**
 * @file TelemetryGaugeWidget.hpp
 * @brief Custom gauge widget for displaying telemetry values
 */

#ifndef TELEMETRYGAUGEWIDGET_HPP
#define TELEMETRYGAUGEWIDGET_HPP

#include <QWidget>
#include <QColor>
#include <QTimer>

/**
 * @class TelemetryGaugeWidget
 * @brief A circular gauge widget for visualizing telemetry values
 */
class TelemetryGaugeWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(double warningThreshold READ warningThreshold WRITE setWarningThreshold)
    Q_PROPERTY(double criticalThreshold READ criticalThreshold WRITE setCriticalThreshold)

public:
    explicit TelemetryGaugeWidget(QWidget *parent = nullptr);
    ~TelemetryGaugeWidget();

    double value() const;
    double warningThreshold() const;
    double criticalThreshold() const;
    double maxValue() const;
    
    void setWarningThreshold(double threshold);
    void setCriticalThreshold(double threshold);
    void setUnit(const QString &unit);
    void setTitle(const QString &title);
    void setMaxValue(double maxValue);

public slots:
    void setValue(double value);
    void setSeverity(const QString &severity);

signals:
    void valueChanged(double value);

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:
    void drawBackground(QPainter &painter);
    void drawArc(QPainter &painter);
    void drawValue(QPainter &painter);
    void drawTitle(QPainter &painter);
    QColor getValueColor() const;

    double m_value;
    double m_targetValue;
    double m_warningThreshold;
    double m_criticalThreshold;
    double m_maxValue;
    QString m_unit;
    QString m_title;
    QString m_severity;
    
    // Colors
    QColor m_backgroundColor;
    QColor m_normalColor;
    QColor m_warningColor;
    QColor m_criticalColor;
    QColor m_textColor;
    
    // Animation
    QTimer *m_animationTimer;
};

#endif // TELEMETRYGAUGEWIDGET_HPP