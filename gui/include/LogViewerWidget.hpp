/**
 * @file LogViewerWidget.hpp
 * @brief Widget for viewing and filtering log messages
 */

#ifndef LOGVIEWERWIDGET_HPP
#define LOGVIEWERWIDGET_HPP

#include <QWidget>
#include <QList>

class QTableWidget;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QPushButton;
class QLabel;
class QVBoxLayout;

/**
 * @class LogViewerWidget
 * @brief Displays log messages with filtering and search capabilities
 * 
 * Features:
 * - Real-time log display
 * - Filter by severity level
 * - Filter by context (CPU, GPU, RAM)
 * - Text search
 * - Auto-scroll option
 * - Export to file
 */
class LogViewerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogViewerWidget(QWidget *parent = nullptr);
    ~LogViewerWidget();

public slots:
    /**
     * @brief Add a log message to the viewer
     * @param timestamp Time of the log
     * @param severity Severity level (INFO, WARNING, CRITICAL)
     * @param context Source context (CPU, GPU, RAM)
     * @param message Log message text
     */
    void addLog(const QString &timestamp, const QString &severity,
                const QString &context, const QString &message);
    
    /**
     * @brief Clear all log messages
     */
    void clearLogs();
    
    /**
     * @brief Export logs to a file
     * @param filePath Path to save the logs
     * @return true if export successful
     */
    bool exportLogs(const QString &filePath);

signals:
    void logCountChanged(int total, int filtered);

private slots:
    void onFilterChanged();
    void onSearchTextChanged(const QString &text);
    void onAutoScrollToggled(bool enabled);
    void onCopySelected();
    void onExportClicked();

private:
    void setupUI();
    void setupConnections();
    void applyFilters();
    void updateStatistics();
    void scrollToBottom();
    QColor getSeverityColor(const QString &severity);

    QVBoxLayout *m_mainLayout;
    
    // Filter controls
    QLineEdit *m_searchEdit;
    QComboBox *m_severityFilter;
    QComboBox *m_contextFilter;
    QCheckBox *m_autoScrollCheck;
    QPushButton *m_clearButton;
    QPushButton *m_exportButton;
    
    // Log table
    QTableWidget *m_logTable;
    
    // Statistics
    QLabel *m_statsLabel;
    int m_totalCount;
    int m_infoCount;
    int m_warningCount;
    int m_criticalCount;
    
    // Settings
    bool m_autoScroll;
    int m_maxLogEntries;
    
    // Log data storage
    struct LogEntry {
        QString timestamp;
        QString severity;
        QString context;
        QString message;
    };
    QList<LogEntry> m_allLogs;
};

#endif // LOGVIEWERWIDGET_HPP