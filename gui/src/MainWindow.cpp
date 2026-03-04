/**
 * @file MainWindow.cpp
 * @brief Implementation of the main application window
 */

#include "MainWindow.hpp"
#include "DashboardWidget.hpp"
#include "LogViewerWidget.hpp"
#include "TelemetryChartWidget.hpp"
#include "SourceStatusWidget.hpp"
#include "ConfigDialog.hpp"
#include "TelemetryAppController.hpp"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QDockWidget>
#include <QAction>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QCloseEvent>
#include <QSettings>
#include <QDateTime>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_isMonitoring(false)
    , m_statusTimer(new QTimer(this))
{
    setWindowTitle("Telemetry Monitor");
    setMinimumSize(1200, 800);
    resize(1400, 900);
    
    // Create controller first
    m_controller = std::make_unique<TelemetryAppController>(this);
    
    setupUI();
    setupConnections();
    loadSettings();
    
    // Start status timer
    m_statusTimer->setInterval(1000);
    connect(m_statusTimer, &QTimer::timeout, this, &MainWindow::updateStatusBar);
}

MainWindow::~MainWindow()
{
    saveSettings();
    if (m_isMonitoring) {
        m_controller->stop();
    }
}

void MainWindow::setupUI()
{
    setupMenuBar();
    setupToolBar();
    setupCentralWidget();
    setupDockWidgets();
    setupStatusBar();
}

void MainWindow::setupMenuBar()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    
    QAction *newAction = fileMenu->addAction(tr("&New Configuration"));
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &MainWindow::onNewConfig);
    
    QAction *openAction = fileMenu->addAction(tr("&Open Configuration..."));
    openAction->setShortcut(QKeySequence::Open);
    connect(openAction, &QAction::triggered, this, &MainWindow::onOpenConfig);
    
    QAction *saveAction = fileMenu->addAction(tr("&Save Configuration"));
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &MainWindow::onSaveConfig);
    
    fileMenu->addSeparator();
    
    QAction *exportAction = fileMenu->addAction(tr("&Export Logs..."));
    exportAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExportLogs);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = fileMenu->addAction(tr("E&xit"));
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    
    // View menu
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    
    // Will add dock widget toggle actions later
    
    // Help menu
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    
    QAction *aboutAction = helpMenu->addAction(tr("&About"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    
    QAction *aboutQtAction = helpMenu->addAction(tr("About &Qt"));
    connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
}

void MainWindow::setupToolBar()
{
    m_toolBar = addToolBar(tr("Main Toolbar"));
    m_toolBar->setMovable(false);
    m_toolBar->setIconSize(QSize(32, 32));
    
    // Start action
    m_startAction = m_toolBar->addAction(QIcon(":/icons/start.png"), tr("Start"));
    m_startAction->setToolTip(tr("Start telemetry monitoring"));
    m_startAction->setShortcut(QKeySequence(Qt::Key_F5));
    connect(m_startAction, &QAction::triggered, this, &MainWindow::onStartClicked);
    
    // Stop action
    m_stopAction = m_toolBar->addAction(QIcon(":/icons/stop.png"), tr("Stop"));
    m_stopAction->setToolTip(tr("Stop telemetry monitoring"));
    m_stopAction->setShortcut(QKeySequence(Qt::Key_F6));
    m_stopAction->setEnabled(false);
    connect(m_stopAction, &QAction::triggered, this, &MainWindow::onStopClicked);
    
    m_toolBar->addSeparator();
    
    // Clear logs action
    m_clearLogsAction = m_toolBar->addAction(QIcon(":/icons/clear.png"), tr("Clear Logs"));
    m_clearLogsAction->setToolTip(tr("Clear all log messages"));
    connect(m_clearLogsAction, &QAction::triggered, this, &MainWindow::onClearLogsClicked);
    
    m_toolBar->addSeparator();
    
    // Settings action
    m_settingsAction = m_toolBar->addAction(QIcon(":/icons/settings.png"), tr("Settings"));
    m_settingsAction->setToolTip(tr("Open configuration settings"));
    m_settingsAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Comma));
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::onSettingsClicked);
}

void MainWindow::setupCentralWidget()
{
    m_tabWidget = new QTabWidget(this);
    
    // Dashboard tab
    m_dashboardWidget = new DashboardWidget(this);
    m_tabWidget->addTab(m_dashboardWidget, QIcon(":/icons/dashboard.png"), tr("Dashboard"));
    
    // Charts tab
    m_chartWidget = new TelemetryChartWidget(this);
    m_tabWidget->addTab(m_chartWidget, QIcon(":/icons/chart.png"), tr("Charts"));
    
    // Logs tab
    m_logViewerWidget = new LogViewerWidget(this);
    m_tabWidget->addTab(m_logViewerWidget, QIcon(":/icons/log.png"), tr("Logs"));
    
    setCentralWidget(m_tabWidget);
}

void MainWindow::setupDockWidgets()
{
    // Sources dock
    m_sourcesDock = new QDockWidget(tr("Sources"), this);
    m_sourcesDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    
    m_sourceStatusWidget = new SourceStatusWidget(this);
    m_sourcesDock->setWidget(m_sourceStatusWidget);
    
    addDockWidget(Qt::RightDockWidgetArea, m_sourcesDock);
    
    // Add toggle action to View menu
    QMenu *viewMenu = menuBar()->findChild<QMenu*>();
    if (viewMenu) {
        viewMenu->addAction(m_sourcesDock->toggleViewAction());
    }
}

void MainWindow::setupStatusBar()
{
    m_statusLabel = new QLabel(tr("Ready"));
    m_statusLabel->setMinimumWidth(150);
    statusBar()->addWidget(m_statusLabel);
    
    statusBar()->addWidget(new QLabel("|"));
    
    m_messageCountLabel = new QLabel(tr("Messages: 0"));
    m_messageCountLabel->setMinimumWidth(120);
    statusBar()->addWidget(m_messageCountLabel);
    
    statusBar()->addWidget(new QLabel("|"));
    
    m_uptimeLabel = new QLabel(tr("Uptime: --:--:--"));
    m_uptimeLabel->setMinimumWidth(120);
    statusBar()->addPermanentWidget(m_uptimeLabel);
}

void MainWindow::setupConnections()
{
    // Controller connections
    connect(m_controller.get(), &TelemetryAppController::telemetryReceived,
            this, &MainWindow::onTelemetryReceived);
    
    connect(m_controller.get(), &TelemetryAppController::logReceived,
            this, &MainWindow::onLogReceived);
    
    connect(m_controller.get(), &TelemetryAppController::sourceStatusChanged,
            this, &MainWindow::onSourceStatusChanged);
    
    connect(m_controller.get(), &TelemetryAppController::errorOccurred,
            this, &MainWindow::onControllerError);
    
    connect(m_controller.get(), &TelemetryAppController::started, this, [this]() {
        setMonitoringEnabled(true);
        m_startTime = QDateTime::currentDateTime();
        m_statusTimer->start();
    });
    
    connect(m_controller.get(), &TelemetryAppController::stopped, this, [this]() {
        setMonitoringEnabled(false);
        m_statusTimer->stop();
    });
}

void MainWindow::loadSettings()
{
    QSettings settings;
    
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/state").toByteArray());
    
    m_currentConfigPath = settings.value("MainWindow/lastConfig").toString();
    if (!m_currentConfigPath.isEmpty()) {
        m_controller->loadConfig(m_currentConfigPath);
    }
    
    updateWindowTitle();
}

void MainWindow::saveSettings()
{
    QSettings settings;
    
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/state", saveState());
    settings.setValue("MainWindow/lastConfig", m_currentConfigPath);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_isMonitoring) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            tr("Confirm Exit"),
            tr("Telemetry monitoring is still running. Do you want to stop and exit?"),
            QMessageBox::Yes | QMessageBox::No
        );
        
        if (reply == QMessageBox::No) {
            event->ignore();
            return;
        }
        
        m_controller->stop();
    }
    
    saveSettings();
    event->accept();
}

void MainWindow::onStartClicked()
{
    if (!m_isMonitoring) {
        // Populate source status widget before starting
        populateSourcesPanel();
        m_controller->start();
    }
}

void MainWindow::onStopClicked()
{
    if (m_isMonitoring) {
        m_controller->stop();
    }
}

void MainWindow::onSettingsClicked()
{
    ConfigDialog dialog(this);
    dialog.setConfig(m_controller->getConfig());
    
    if (dialog.exec() == QDialog::Accepted) {
        bool wasRunning = m_isMonitoring;
        
        if (wasRunning) {
            m_controller->stop();
        }
        
        m_controller->setConfig(dialog.getConfig());
        
        // Update sources panel
        populateSourcesPanel();
        
        if (wasRunning) {
            m_controller->start();
        }
    }
}

void MainWindow::onClearLogsClicked()
{
    m_logViewerWidget->clearLogs();
    m_chartWidget->clearChart();
    m_dashboardWidget->clearAll();
    m_messageCountLabel->setText(tr("Messages: 0"));
}

void MainWindow::onNewConfig()
{
    if (m_isMonitoring) {
        m_controller->stop();
    }
    
    m_controller->setConfig(AppConfigData());
    m_currentConfigPath.clear();
    updateWindowTitle();
}

void MainWindow::onOpenConfig()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open Configuration"),
        QString(),
        tr("JSON Files (*.json);;All Files (*)")
    );
    
    if (!filePath.isEmpty()) {
        if (m_controller->loadConfig(filePath)) {
            m_currentConfigPath = filePath;
            updateWindowTitle();
            
            // Populate source status widget
            populateSourcesPanel();
        } else {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Failed to load configuration file."));
        }
    }
}

void MainWindow::populateSourcesPanel()
{
    m_sourceStatusWidget->clearSources();
    
    AppConfigData config = m_controller->getConfig();
    for (const auto &source : config.sources) {
        QString sourceName = source.telemetryType + " (" + source.type + ")";
        m_sourceStatusWidget->addSource(sourceName, source.type);
    }
}

void MainWindow::onSaveConfig()
{
    if (m_currentConfigPath.isEmpty()) {
        QString filePath = QFileDialog::getSaveFileName(
            this,
            tr("Save Configuration"),
            QString(),
            tr("JSON Files (*.json);;All Files (*)")
        );
        
        if (filePath.isEmpty()) {
            return;
        }
        m_currentConfigPath = filePath;
    }
    
    if (m_controller->saveConfig(m_currentConfigPath)) {
        updateWindowTitle();
        statusBar()->showMessage(tr("Configuration saved"), 3000);
    } else {
        QMessageBox::warning(this, tr("Error"),
                             tr("Failed to save configuration file."));
    }
}

void MainWindow::onExportLogs()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Export Logs"),
        QString(),
        tr("Text Files (*.txt);;CSV Files (*.csv);;All Files (*)")
    );
    
    if (!filePath.isEmpty()) {
        if (m_logViewerWidget->exportLogs(filePath)) {
            statusBar()->showMessage(tr("Logs exported successfully"), 3000);
        } else {
            QMessageBox::warning(this, tr("Error"),
                                 tr("Failed to export logs."));
        }
    }
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, tr("About Telemetry Monitor"),
        tr("<h2>Telemetry Monitor</h2>"
           "<p>Version 1.0.0</p>"
           "<p>A real-time telemetry monitoring application.</p>"
           "<p>Features:</p>"
           "<ul>"
           "<li>Multiple data sources (File, Socket, SOME/IP)</li>"
           "<li>Real-time gauges and charts</li>"
           "<li>Configurable thresholds and alerts</li>"
           "<li>Log viewing and filtering</li>"
           "</ul>"));
}

void MainWindow::onTelemetryReceived(const QString &type, double value, 
                                      const QString &severity)
{
    // Update dashboard and chart
    m_dashboardWidget->updateTelemetry(type, value, severity);
    m_chartWidget->addDataPoint(type, value);
    
    // Update source status - find the source for this telemetry type
    AppConfigData config = m_controller->getConfig();
    for (const auto &source : config.sources) {
        if (source.telemetryType == type) {
            QString sourceName = source.telemetryType + " (" + source.type + ")";
            m_sourceStatusWidget->setSourceStatus(sourceName, true);
            m_sourceStatusWidget->updateLastDataTime(sourceName);
            break;
        }
    }
}

void MainWindow::onLogReceived(const QString &timestamp, const QString &severity,
                                const QString &context, const QString &message)
{
    m_logViewerWidget->addLog(timestamp, severity, context, message);
    
    // Update message count
    static int messageCount = 0;
    messageCount++;
    m_messageCountLabel->setText(tr("Messages: %1").arg(messageCount));
}

void MainWindow::onSourceStatusChanged(const QString &sourceName, bool isActive)
{
    m_sourceStatusWidget->setSourceStatus(sourceName, isActive);
}

void MainWindow::onControllerError(const QString &error)
{
    QMessageBox::critical(this, tr("Error"), error);
    m_statusLabel->setText(tr("Error: %1").arg(error));
}

void MainWindow::updateStatusBar()
{
    if (m_isMonitoring && m_startTime.isValid()) {
        qint64 seconds = m_startTime.secsTo(QDateTime::currentDateTime());
        int hours = seconds / 3600;
        int minutes = (seconds % 3600) / 60;
        int secs = seconds % 60;
        
        m_uptimeLabel->setText(tr("Uptime: %1:%2:%3")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0')));
    }
}

void MainWindow::setMonitoringEnabled(bool enabled)
{
    m_isMonitoring = enabled;
    m_startAction->setEnabled(!enabled);
    m_stopAction->setEnabled(enabled);
    m_settingsAction->setEnabled(!enabled);
    
    m_statusLabel->setText(enabled ? tr("Monitoring...") : tr("Stopped"));
    updateWindowTitle();
}

void MainWindow::updateWindowTitle()
{
    QString title = "Telemetry Monitor";
    
    if (!m_currentConfigPath.isEmpty()) {
        QFileInfo fileInfo(m_currentConfigPath);
        title += " - " + fileInfo.fileName();
    }
    
    if (m_isMonitoring) {
        title += " [Running]";
    }
    
    setWindowTitle(title);
}