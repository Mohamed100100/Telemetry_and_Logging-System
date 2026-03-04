/**
 * @file MainWindow.hpp
 * @brief Main application window containing all GUI components
 */

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QTimer>
#include <QDateTime> 
#include <memory>

class QToolBar;
class QStatusBar;
class QAction;
class QLabel;
class QTabWidget;
class QDockWidget;

class DashboardWidget;
class LogViewerWidget;
class TelemetryChartWidget;
class SourceStatusWidget;
class ConfigDialog;
class TelemetryAppController;

/**
 * @class MainWindow
 * @brief Main application window - the central hub of the GUI
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // Toolbar actions
    void onStartClicked();
    void onStopClicked();
    void onSettingsClicked();
    void onClearLogsClicked();
    
    // Menu actions
    void onNewConfig();
    void onOpenConfig();
    void onSaveConfig();
    void onExportLogs();
    void onAbout();
    
    // Controller signals
    void onTelemetryReceived(const QString &type, double value, const QString &severity);
    void onLogReceived(const QString &timestamp, const QString &severity,
                       const QString &context, const QString &message);
    void onSourceStatusChanged(const QString &sourceName, bool isActive);
    void onControllerError(const QString &error);
    
    // Status updates
    void updateStatusBar();

private:
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupCentralWidget();
    void setupDockWidgets();
    void setupStatusBar();
    void setupConnections();
    
    void loadSettings();
    void saveSettings();
    void setMonitoringEnabled(bool enabled);
    void updateWindowTitle();
    void populateSourcesPanel();

    // UI Components
    QToolBar *m_toolBar;
    QTabWidget *m_tabWidget;
    
    // Main widgets
    DashboardWidget *m_dashboardWidget;
    LogViewerWidget *m_logViewerWidget;
    TelemetryChartWidget *m_chartWidget;
    
    // Dock widgets
    QDockWidget *m_sourcesDock;
    SourceStatusWidget *m_sourceStatusWidget;
    
    // Actions
    QAction *m_startAction;
    QAction *m_stopAction;
    QAction *m_settingsAction;
    QAction *m_clearLogsAction;
    
    // Status bar widgets
    QLabel *m_statusLabel;
    QLabel *m_messageCountLabel;
    QLabel *m_uptimeLabel;
    
    // Controller
    std::unique_ptr<TelemetryAppController> m_controller;
    
    // State
    bool m_isMonitoring;
    QString m_currentConfigPath;
    QTimer *m_statusTimer;
    QDateTime m_startTime;
};

#endif // MAINWINDOW_HPP