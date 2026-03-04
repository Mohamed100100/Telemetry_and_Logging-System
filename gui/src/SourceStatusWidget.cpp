/**
 * @file SourceStatusWidget.cpp
 * @brief Implementation of the source status widget
 */

#include "SourceStatusWidget.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QDateTime>
#include <QDebug>

SourceStatusWidget::SourceStatusWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

SourceStatusWidget::~SourceStatusWidget()
{
}

void SourceStatusWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    
    QLabel *titleLabel = new QLabel("Data Sources");
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    m_mainLayout->addWidget(titleLabel);
    
    // Add stretch at bottom
    m_mainLayout->addStretch();
}

void SourceStatusWidget::addSource(const QString &name, const QString &type)
{
    if (m_sources.contains(name)) {
        return;
    }
    
    QFrame *card = createSourceCard(name, type);
    
    // Insert before the stretch
    m_mainLayout->insertWidget(m_mainLayout->count() - 1, card);
    
    SourceInfo info;
    info.type = type;
    info.isConnected = false;
    info.dataCount = 0;
    info.card = card;
    info.statusLabel = card->findChild<QLabel*>("statusLabel");
    info.lastDataLabel = card->findChild<QLabel*>("lastDataLabel");
    info.rateLabel = card->findChild<QLabel*>("rateLabel");
    
    m_sources[name] = info;
    
    qDebug() << "Added source to widget:" << name;
}

void SourceStatusWidget::removeSource(const QString &name)
{
    if (!m_sources.contains(name)) {
        return;
    }
    
    QFrame *card = m_sources[name].card;
    m_mainLayout->removeWidget(card);
    delete card;
    
    m_sources.remove(name);
}

void SourceStatusWidget::setSourceStatus(const QString &name, bool isConnected)
{
    if (!m_sources.contains(name)) {
        qDebug() << "setSourceStatus: Source not found:" << name;
        return;
    }
    
    m_sources[name].isConnected = isConnected;
    updateSourceCard(name);
}

void SourceStatusWidget::updateLastDataTime(const QString &name)
{
    if (!m_sources.contains(name)) {
        qDebug() << "updateLastDataTime: Source not found:" << name;
        return;
    }
    
    SourceInfo &info = m_sources[name];
    info.lastDataTime = QDateTime::currentDateTime();
    info.dataCount++;  // Increment count
    
    qDebug() << "Updated data count for" << name << ":" << info.dataCount;
    
    updateSourceCard(name);
}

void SourceStatusWidget::clearSources()
{
    QStringList names = m_sources.keys();
    for (const QString &name : names) {
        removeSource(name);
    }
}

QFrame* SourceStatusWidget::createSourceCard(const QString &name, const QString &type)
{
    QFrame *card = new QFrame();
    card->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    card->setStyleSheet(
        "QFrame {"
        "  background-color: #3a3a3a;"
        "  border-radius: 5px;"
        "  padding: 10px;"
        "}"
    );
    
    QVBoxLayout *layout = new QVBoxLayout(card);
    layout->setSpacing(5);
    
    // Header row with name and status indicator
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    QLabel *nameLabel = new QLabel(name);
    nameLabel->setStyleSheet("font-weight: bold;");
    headerLayout->addWidget(nameLabel);
    
    headerLayout->addStretch();
    
    QLabel *statusLabel = new QLabel("●");
    statusLabel->setObjectName("statusLabel");
    statusLabel->setStyleSheet("color: #888; font-size: 16px;");
    headerLayout->addWidget(statusLabel);
    
    layout->addLayout(headerLayout);
    
    // Type label
    QLabel *typeLabel = new QLabel(QString("Type: %1").arg(type));
    typeLabel->setStyleSheet("color: #aaa; font-size: 11px;");
    layout->addWidget(typeLabel);
    
    // Last data label
    QLabel *lastDataLabel = new QLabel("Last data: --");
    lastDataLabel->setObjectName("lastDataLabel");
    lastDataLabel->setStyleSheet("color: #aaa; font-size: 11px;");
    layout->addWidget(lastDataLabel);
    
    // Rate/Count label
    QLabel *rateLabel = new QLabel("Count: 0");
    rateLabel->setObjectName("rateLabel");
    rateLabel->setStyleSheet("color: #aaa; font-size: 11px;");
    layout->addWidget(rateLabel);
    
    return card;
}

void SourceStatusWidget::updateSourceCard(const QString &name)
{
    if (!m_sources.contains(name)) {
        return;
    }
    
    SourceInfo &info = m_sources[name];
    
    // Update status indicator
    if (info.statusLabel) {
        if (info.isConnected) {
            info.statusLabel->setStyleSheet("color: #27ae60; font-size: 16px;");
            info.statusLabel->setToolTip("Connected");
        } else {
            info.statusLabel->setStyleSheet("color: #e74c3c; font-size: 16px;");
            info.statusLabel->setToolTip("Disconnected");
        }
    }
    
    // Update last data time
    if (info.lastDataLabel) {
        if (info.lastDataTime.isValid()) {
            info.lastDataLabel->setText(
                QString("Last data: %1").arg(info.lastDataTime.toString("hh:mm:ss")));
        } else {
            info.lastDataLabel->setText("Last data: --");
        }
    }
    
    // Update count
    if (info.rateLabel) {
        info.rateLabel->setText(QString("Count: %1").arg(info.dataCount));
    }
}