/**
 * @file LogViewerWidget.cpp
 * @brief Implementation of the log viewer widget
 */

#include "LogViewerWidget.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QTextStream>
#include <QClipboard>
#include <QApplication>
#include <QMenu>
#include <QScrollBar>

LogViewerWidget::LogViewerWidget(QWidget *parent)
    : QWidget(parent)
    , m_totalCount(0)
    , m_infoCount(0)
    , m_warningCount(0)
    , m_criticalCount(0)
    , m_autoScroll(true)
    , m_maxLogEntries(10000)
{
    setupUI();
    setupConnections();
}

LogViewerWidget::~LogViewerWidget()
{
}

void LogViewerWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    
    // Filter bar
    QHBoxLayout *filterLayout = new QHBoxLayout();
    
    // Search box
    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Search logs...");
    m_searchEdit->setClearButtonEnabled(true);
    m_searchEdit->setMinimumWidth(200);
    filterLayout->addWidget(m_searchEdit);
    
    // Severity filter
    filterLayout->addWidget(new QLabel("Severity:"));
    m_severityFilter = new QComboBox();
    m_severityFilter->addItem("All", "");
    m_severityFilter->addItem("Info", "INFO");
    m_severityFilter->addItem("Warning", "WARNING");
    m_severityFilter->addItem("Critical", "CRITICAL");
    m_severityFilter->setMinimumWidth(100);
    filterLayout->addWidget(m_severityFilter);
    
    // Context filter
    filterLayout->addWidget(new QLabel("Context:"));
    m_contextFilter = new QComboBox();
    m_contextFilter->addItem("All", "");
    m_contextFilter->addItem("CPU", "CPU");
    m_contextFilter->addItem("GPU", "GPU");
    m_contextFilter->addItem("RAM", "RAM");
    m_contextFilter->setMinimumWidth(100);
    filterLayout->addWidget(m_contextFilter);
    
    filterLayout->addStretch();
    
    // Auto-scroll checkbox
    m_autoScrollCheck = new QCheckBox("Auto-scroll");
    m_autoScrollCheck->setChecked(m_autoScroll);
    filterLayout->addWidget(m_autoScrollCheck);
    
    // Clear button
    m_clearButton = new QPushButton("Clear");
    m_clearButton->setIcon(QIcon(":/icons/clear.png"));
    filterLayout->addWidget(m_clearButton);
    
    // Export button
    m_exportButton = new QPushButton("Export");
    m_exportButton->setIcon(QIcon(":/icons/save.png"));
    filterLayout->addWidget(m_exportButton);
    
    m_mainLayout->addLayout(filterLayout);
    
    // Log table
    m_logTable = new QTableWidget();
    m_logTable->setColumnCount(4);
    m_logTable->setHorizontalHeaderLabels({"Timestamp", "Severity", "Context", "Message"});
    m_logTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_logTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_logTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_logTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_logTable->verticalHeader()->setVisible(false);
    m_logTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_logTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_logTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_logTable->setAlternatingRowColors(true);
    m_logTable->setContextMenuPolicy(Qt::CustomContextMenu);
    
    m_mainLayout->addWidget(m_logTable, 1);
    
    // Statistics bar
    QHBoxLayout *statsLayout = new QHBoxLayout();
    
    m_statsLabel = new QLabel();
    updateStatistics();
    statsLayout->addWidget(m_statsLabel);
    
    statsLayout->addStretch();
    
    m_mainLayout->addLayout(statsLayout);
}

void LogViewerWidget::setupConnections()
{
    connect(m_searchEdit, &QLineEdit::textChanged,
            this, &LogViewerWidget::onSearchTextChanged);
    
    connect(m_severityFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LogViewerWidget::onFilterChanged);
    
    connect(m_contextFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LogViewerWidget::onFilterChanged);
    
    connect(m_autoScrollCheck, &QCheckBox::toggled,
            this, &LogViewerWidget::onAutoScrollToggled);
    
    connect(m_clearButton, &QPushButton::clicked,
            this, &LogViewerWidget::clearLogs);
    
    connect(m_exportButton, &QPushButton::clicked,
            this, &LogViewerWidget::onExportClicked);
    
    connect(m_logTable, &QTableWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        QMenu menu;
        QAction *copyAction = menu.addAction("Copy");
        connect(copyAction, &QAction::triggered, this, &LogViewerWidget::onCopySelected);
        menu.exec(m_logTable->mapToGlobal(pos));
    });
}

void LogViewerWidget::addLog(const QString &timestamp, const QString &severity,
                              const QString &context, const QString &message)
{
    // Store log entry
    LogEntry entry;
    entry.timestamp = timestamp;
    entry.severity = severity;
    entry.context = context;
    entry.message = message;
    m_allLogs.append(entry);
    
    // Update counters
    m_totalCount++;
    if (severity == "INFO") m_infoCount++;
    else if (severity == "WARNING") m_warningCount++;
    else if (severity == "CRITICAL") m_criticalCount++;
    
    // Check if entry matches current filter
    QString searchText = m_searchEdit->text().toLower();
    QString severityFilterValue = m_severityFilter->currentData().toString();
    QString contextFilterValue = m_contextFilter->currentData().toString();
    
    bool matches = true;
    
    if (!searchText.isEmpty()) {
        matches = message.toLower().contains(searchText) ||
                  context.toLower().contains(searchText);
    }
    
    if (matches && !severityFilterValue.isEmpty()) {
        matches = (severity == severityFilterValue);
    }
    
    if (matches && !contextFilterValue.isEmpty()) {
        matches = (context == contextFilterValue);
    }
    
    // Add to table if matches filter
    if (matches) {
        int row = m_logTable->rowCount();
        m_logTable->insertRow(row);
        
        QTableWidgetItem *timestampItem = new QTableWidgetItem(timestamp);
        QTableWidgetItem *severityItem = new QTableWidgetItem(severity);
        QTableWidgetItem *contextItem = new QTableWidgetItem(context);
        QTableWidgetItem *messageItem = new QTableWidgetItem(message);
        
        // Set severity color
        QColor severityColor = getSeverityColor(severity);
        severityItem->setForeground(severityColor);
        severityItem->setFont(QFont(severityItem->font().family(), -1, QFont::Bold));
        
        m_logTable->setItem(row, 0, timestampItem);
        m_logTable->setItem(row, 1, severityItem);
        m_logTable->setItem(row, 2, contextItem);
        m_logTable->setItem(row, 3, messageItem);
        
        if (m_autoScroll) {
            scrollToBottom();
        }
    }
    
    // Limit log entries
    while (m_allLogs.size() > m_maxLogEntries) {
        m_allLogs.removeFirst();
    }
    
    updateStatistics();
    emit logCountChanged(m_totalCount, m_logTable->rowCount());
}

void LogViewerWidget::clearLogs()
{
    m_logTable->setRowCount(0);
    m_allLogs.clear();
    m_totalCount = 0;
    m_infoCount = 0;
    m_warningCount = 0;
    m_criticalCount = 0;
    updateStatistics();
    emit logCountChanged(0, 0);
}

bool LogViewerWidget::exportLogs(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream out(&file);
    
    // Write header
    out << "Timestamp,Severity,Context,Message\n";
    
    // Write all logs
    for (const LogEntry &entry : m_allLogs) {
        // Create a copy of message for escaping quotes
        QString escapedMessage = entry.message;
        escapedMessage.replace("\"", "\"\"");
        
        out << "\"" << entry.timestamp << "\","
            << "\"" << entry.severity << "\","
            << "\"" << entry.context << "\","
            << "\"" << escapedMessage << "\"\n";
    }
    
    file.close();
    return true;
}

void LogViewerWidget::onFilterChanged()
{
    applyFilters();
}

void LogViewerWidget::onSearchTextChanged(const QString &text)
{
    Q_UNUSED(text)
    applyFilters();
}

void LogViewerWidget::onAutoScrollToggled(bool enabled)
{
    m_autoScroll = enabled;
    if (enabled) {
        scrollToBottom();
    }
}

void LogViewerWidget::onCopySelected()
{
    QList<QTableWidgetItem*> items = m_logTable->selectedItems();
    if (items.isEmpty()) return;
    
    QSet<int> selectedRows;
    for (QTableWidgetItem *item : items) {
        selectedRows.insert(item->row());
    }
    
    QString text;
    for (int row : selectedRows) {
        text += m_logTable->item(row, 0)->text() + "\t";
        text += m_logTable->item(row, 1)->text() + "\t";
        text += m_logTable->item(row, 2)->text() + "\t";
        text += m_logTable->item(row, 3)->text() + "\n";
    }
    
    QApplication::clipboard()->setText(text);
}

void LogViewerWidget::onExportClicked()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        "Export Logs",
        QString(),
        "CSV Files (*.csv);;Text Files (*.txt);;All Files (*)"
    );
    
    if (!filePath.isEmpty()) {
        exportLogs(filePath);
    }
}

void LogViewerWidget::applyFilters()
{
    QString searchText = m_searchEdit->text().toLower();
    QString severityFilterValue = m_severityFilter->currentData().toString();
    QString contextFilterValue = m_contextFilter->currentData().toString();
    
    // Clear and repopulate table
    m_logTable->setRowCount(0);
    
    for (const LogEntry &entry : m_allLogs) {
        bool matches = true;
        
        if (!searchText.isEmpty()) {
            matches = entry.message.toLower().contains(searchText) ||
                      entry.context.toLower().contains(searchText);
        }
        
        if (matches && !severityFilterValue.isEmpty()) {
            matches = (entry.severity == severityFilterValue);
        }
        
        if (matches && !contextFilterValue.isEmpty()) {
            matches = (entry.context == contextFilterValue);
        }
        
        if (matches) {
            int row = m_logTable->rowCount();
            m_logTable->insertRow(row);
            
            QTableWidgetItem *timestampItem = new QTableWidgetItem(entry.timestamp);
            QTableWidgetItem *severityItem = new QTableWidgetItem(entry.severity);
            QTableWidgetItem *contextItem = new QTableWidgetItem(entry.context);
            QTableWidgetItem *messageItem = new QTableWidgetItem(entry.message);
            
            QColor severityColor = getSeverityColor(entry.severity);
            severityItem->setForeground(severityColor);
            severityItem->setFont(QFont(severityItem->font().family(), -1, QFont::Bold));
            
            m_logTable->setItem(row, 0, timestampItem);
            m_logTable->setItem(row, 1, severityItem);
            m_logTable->setItem(row, 2, contextItem);
            m_logTable->setItem(row, 3, messageItem);
        }
    }
    
    emit logCountChanged(m_totalCount, m_logTable->rowCount());
}

void LogViewerWidget::updateStatistics()
{
    m_statsLabel->setText(QString("Total: %1 | Info: %2 | Warnings: %3 | Critical: %4")
        .arg(m_totalCount)
        .arg(m_infoCount)
        .arg(m_warningCount)
        .arg(m_criticalCount));
}

void LogViewerWidget::scrollToBottom()
{
    m_logTable->scrollToBottom();
}

QColor LogViewerWidget::getSeverityColor(const QString &severity)
{
    if (severity == "CRITICAL") {
        return QColor(231, 76, 60);
    } else if (severity == "WARNING") {
        return QColor(243, 156, 18);
    }
    return QColor(46, 204, 113);
}