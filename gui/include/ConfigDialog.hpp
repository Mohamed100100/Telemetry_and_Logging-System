/**
 * @file ConfigDialog.hpp
 * @brief Configuration dialog for application settings
 */

#ifndef CONFIGDIALOG_HPP
#define CONFIGDIALOG_HPP

#include <QDialog>
#include <QList>

class QTabWidget;
class QLineEdit;
class QSpinBox;
class QComboBox;
class QListWidget;
class QPushButton;
class QCheckBox;
class QDialogButtonBox;

/**
 * @struct SourceConfigData
 * @brief Configuration data for a telemetry source
 */
struct SourceConfigData {
    QString type;           // "file", "socket", "someip"
    QString path;           // File/socket path
    QString telemetryType;  // "CPU", "GPU", "RAM"
    int rateMs;             // Polling rate in milliseconds
};

/**
 * @struct SinkConfigData
 * @brief Configuration data for a log sink
 */
struct SinkConfigData {
    QString type;   // "console", "file"
    QString path;   // File path (for file sink)
};

/**
 * @struct AppConfigData
 * @brief Complete application configuration
 */
struct AppConfigData {
    QString appName;
    int bufferSize;
    int threadPoolSize;
    QList<SourceConfigData> sources;
    QList<SinkConfigData> sinks;
};

/**
 * @class ConfigDialog
 * @brief Dialog for configuring the telemetry application
 * 
 * Allows configuration of:
 * - Application settings (name, buffer size, thread pool)
 * - Telemetry sources (file, socket, SOME/IP)
 * - Log sinks (console, file)
 */
class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr);
    ~ConfigDialog();

    /**
     * @brief Set the configuration to display/edit
     */
    void setConfig(const AppConfigData &config);
    
    /**
     * @brief Get the current configuration
     */
    AppConfigData getConfig() const;
    
    /**
     * @brief Load configuration from JSON file
     */
    bool loadFromFile(const QString &filePath);
    
    /**
     * @brief Save configuration to JSON file
     */
    bool saveToFile(const QString &filePath);

signals:
    void configurationChanged();

private slots:
    void onAddSource();
    void onRemoveSource();
    void onEditSource();
    void onSourceDoubleClicked();
    
    void onAddSink();
    void onRemoveSink();
    void onEditSink();
    void onSinkDoubleClicked();
    
    void onBrowseFilePath();
    void onAccepted();

private:
    void setupUI();
    void setupGeneralTab();
    void setupSourcesTab();
    void setupSinksTab();
    void updateSourcesList();
    void updateSinksList();
    bool validateConfig();

    QTabWidget *m_tabWidget;
    QDialogButtonBox *m_buttonBox;
    
    // General tab
    QLineEdit *m_appNameEdit;
    QSpinBox *m_bufferSizeSpinBox;
    QSpinBox *m_threadPoolSpinBox;
    
    // Sources tab
    QListWidget *m_sourcesList;
    QPushButton *m_addSourceBtn;
    QPushButton *m_removeSourceBtn;
    QPushButton *m_editSourceBtn;
    
    // Sinks tab
    QListWidget *m_sinksList;
    QPushButton *m_addSinkBtn;
    QPushButton *m_removeSinkBtn;
    QPushButton *m_editSinkBtn;
    
    // Configuration data
    AppConfigData m_config;
};

#endif // CONFIGDIALOG_HPP