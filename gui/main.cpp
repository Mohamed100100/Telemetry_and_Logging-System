/**
 * @file main.cpp
 * @brief Main entry point for the Telemetry GUI Application
 */

#include <QApplication>
#include <QFile>
#include <QStyleFactory>
#include <QFontDatabase>
#include <QScreen>

#include "MainWindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    QApplication::setApplicationName("Telemetry Monitor");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("TelemetrySystem");
    
    // Use Fusion style for consistent look across platforms
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Load dark theme stylesheet
    QFile styleFile(":/styles/dark_theme.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }
    
    // Create and show main window
    MainWindow mainWindow;
    
    // Center on screen
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int x = (screenGeometry.width() - mainWindow.width()) / 2;
    int y = (screenGeometry.height() - mainWindow.height()) / 2;
    mainWindow.move(x, y);
    
    mainWindow.show();
    
    return app.exec();
}