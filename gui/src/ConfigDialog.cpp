/**
 * @file ConfigDialog.cpp
 * @brief Implementation of the configuration dialog
 */

#include "ConfigDialog.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

ConfigDialog::ConfigDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Configuration");
    setMinimumSize(500, 400);
    resize(600, 500);
    
    setupUI();
    
    // Set default values
    m_config.appName = "TelemetryApp";
    m_config.bufferSize = 100;
    m_config.threadPoolSize = 4;
}

ConfigDialog::~ConfigDialog()
{
}

void ConfigDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    m_tabWidget = new QTabWidget();
    mainLayout->addWidget(m_tabWidget);
    
    setupGeneralTab();
    setupSourcesTab();
    setupSinksTab();
    
    // Button box
    m_buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &ConfigDialog::onAccepted);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(m_buttonBox);
}

void ConfigDialog::setupGeneralTab()
{
    QWidget *tab = new QWidget();
    QFormLayout *layout = new QFormLayout(tab);
    layout->setSpacing(15);
    
    m_appNameEdit = new QLineEdit();
    m_appNameEdit->setText("TelemetryApp");
    layout->addRow("Application Name:", m_appNameEdit);
    
    m_bufferSizeSpinBox = new QSpinBox();
    m_bufferSizeSpinBox->setRange(10, 10000);
    m_bufferSizeSpinBox->setValue(100);
    m_bufferSizeSpinBox->setSuffix(" messages");
    layout->addRow("Buffer Size:", m_bufferSizeSpinBox);
    
    m_threadPoolSpinBox = new QSpinBox();
    m_threadPoolSpinBox->setRange(1, 16);
    m_threadPoolSpinBox->setValue(4);
    m_threadPoolSpinBox->setSuffix(" threads");
    layout->addRow("Thread Pool Size:", m_threadPoolSpinBox);
    
    m_tabWidget->addTab(tab, "General");
}

void ConfigDialog::setupSourcesTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    m_sourcesList = new QListWidget();
    layout->addWidget(m_sourcesList);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_addSourceBtn = new QPushButton("Add");
    connect(m_addSourceBtn, &QPushButton::clicked, this, &ConfigDialog::onAddSource);
    buttonLayout->addWidget(m_addSourceBtn);
    
    m_editSourceBtn = new QPushButton("Edit");
    m_editSourceBtn->setEnabled(false);
    connect(m_editSourceBtn, &QPushButton::clicked, this, &ConfigDialog::onEditSource);
    buttonLayout->addWidget(m_editSourceBtn);
    
    m_removeSourceBtn = new QPushButton("Remove");
    m_removeSourceBtn->setEnabled(false);
    connect(m_removeSourceBtn, &QPushButton::clicked, this, &ConfigDialog::onRemoveSource);
    buttonLayout->addWidget(m_removeSourceBtn);
    
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);
    
    connect(m_sourcesList, &QListWidget::itemSelectionChanged, this, [this]() {
        bool hasSelection = !m_sourcesList->selectedItems().isEmpty();
        m_editSourceBtn->setEnabled(hasSelection);
        m_removeSourceBtn->setEnabled(hasSelection);
    });
    
    connect(m_sourcesList, &QListWidget::itemDoubleClicked, 
            this, &ConfigDialog::onSourceDoubleClicked);
    
    m_tabWidget->addTab(tab, "Sources");
}

void ConfigDialog::setupSinksTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    
    m_sinksList = new QListWidget();
    layout->addWidget(m_sinksList);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    m_addSinkBtn = new QPushButton("Add");
    connect(m_addSinkBtn, &QPushButton::clicked, this, &ConfigDialog::onAddSink);
    buttonLayout->addWidget(m_addSinkBtn);
    
    m_editSinkBtn = new QPushButton("Edit");
    m_editSinkBtn->setEnabled(false);
    connect(m_editSinkBtn, &QPushButton::clicked, this, &ConfigDialog::onEditSink);
    buttonLayout->addWidget(m_editSinkBtn);
    
    m_removeSinkBtn = new QPushButton("Remove");
    m_removeSinkBtn->setEnabled(false);
    connect(m_removeSinkBtn, &QPushButton::clicked, this, &ConfigDialog::onRemoveSink);
    buttonLayout->addWidget(m_removeSinkBtn);
    
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);
    
    connect(m_sinksList, &QListWidget::itemSelectionChanged, this, [this]() {
        bool hasSelection = !m_sinksList->selectedItems().isEmpty();
        m_editSinkBtn->setEnabled(hasSelection);
        m_removeSinkBtn->setEnabled(hasSelection);
    });
    
    connect(m_sinksList, &QListWidget::itemDoubleClicked,
            this, &ConfigDialog::onSinkDoubleClicked);
    
    m_tabWidget->addTab(tab, "Sinks");
}

void ConfigDialog::setConfig(const AppConfigData &config)
{
    m_config = config;
    
    m_appNameEdit->setText(config.appName);
    m_bufferSizeSpinBox->setValue(config.bufferSize);
    m_threadPoolSpinBox->setValue(config.threadPoolSize);
    
    updateSourcesList();
    updateSinksList();
}

AppConfigData ConfigDialog::getConfig() const
{
    AppConfigData config = m_config;
    config.appName = m_appNameEdit->text();
    config.bufferSize = m_bufferSizeSpinBox->value();
    config.threadPoolSize = m_threadPoolSpinBox->value();
    return config;
}

void ConfigDialog::updateSourcesList()
{
    m_sourcesList->clear();
    for (const SourceConfigData &source : m_config.sources) {
        QString text = QString("%1 (%2) - %3ms")
            .arg(source.telemetryType)
            .arg(source.type)
            .arg(source.rateMs);
        if (!source.path.isEmpty()) {
            text += " - " + source.path;
        }
        m_sourcesList->addItem(text);
    }
}

void ConfigDialog::updateSinksList()
{
    m_sinksList->clear();
    for (const SinkConfigData &sink : m_config.sinks) {
        QString text = sink.type;
        if (!sink.path.isEmpty()) {
            text += " - " + sink.path;
        }
        m_sinksList->addItem(text);
    }
}

void ConfigDialog::onAddSource()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Add Source");
    
    QFormLayout *layout = new QFormLayout(&dialog);
    
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItems({"file", "socket", "someip"});
    layout->addRow("Type:", typeCombo);
    
    QLineEdit *pathEdit = new QLineEdit();
    layout->addRow("Path:", pathEdit);
    
    QComboBox *telemetryCombo = new QComboBox();
    telemetryCombo->addItems({"CPU", "GPU", "RAM"});
    layout->addRow("Telemetry Type:", telemetryCombo);
    
    QSpinBox *rateSpin = new QSpinBox();
    rateSpin->setRange(100, 10000);
    rateSpin->setValue(500);
    rateSpin->setSuffix(" ms");
    layout->addRow("Rate:", rateSpin);
    
    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addRow(buttons);
    
    if (dialog.exec() == QDialog::Accepted) {
        SourceConfigData source;
        source.type = typeCombo->currentText();
        source.path = pathEdit->text();
        source.telemetryType = telemetryCombo->currentText();
        source.rateMs = rateSpin->value();
        
        m_config.sources.append(source);
        updateSourcesList();
        emit configurationChanged();
    }
}

void ConfigDialog::onRemoveSource()
{
    int row = m_sourcesList->currentRow();
    if (row >= 0 && row < m_config.sources.size()) {
        m_config.sources.removeAt(row);
        updateSourcesList();
        emit configurationChanged();
    }
}

void ConfigDialog::onEditSource()
{
    onSourceDoubleClicked();
}

void ConfigDialog::onSourceDoubleClicked()
{
    int row = m_sourcesList->currentRow();
    if (row < 0 || row >= m_config.sources.size()) return;
    
    SourceConfigData &source = m_config.sources[row];
    
    QDialog dialog(this);
    dialog.setWindowTitle("Edit Source");
    
    QFormLayout *layout = new QFormLayout(&dialog);
    
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItems({"file", "socket", "someip"});
    typeCombo->setCurrentText(source.type);
    layout->addRow("Type:", typeCombo);
    
    QLineEdit *pathEdit = new QLineEdit(source.path);
    layout->addRow("Path:", pathEdit);
    
    QComboBox *telemetryCombo = new QComboBox();
    telemetryCombo->addItems({"CPU", "GPU", "RAM"});
    telemetryCombo->setCurrentText(source.telemetryType);
    layout->addRow("Telemetry Type:", telemetryCombo);
    
    QSpinBox *rateSpin = new QSpinBox();
    rateSpin->setRange(100, 10000);
    rateSpin->setValue(source.rateMs);
    rateSpin->setSuffix(" ms");
    layout->addRow("Rate:", rateSpin);
    
    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addRow(buttons);
    
    if (dialog.exec() == QDialog::Accepted) {
        source.type = typeCombo->currentText();
        source.path = pathEdit->text();
        source.telemetryType = telemetryCombo->currentText();
        source.rateMs = rateSpin->value();
        
        updateSourcesList();
        emit configurationChanged();
    }
}

void ConfigDialog::onAddSink()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Add Sink");
    
    QFormLayout *layout = new QFormLayout(&dialog);
    
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItems({"console", "file"});
    layout->addRow("Type:", typeCombo);
    
    QLineEdit *pathEdit = new QLineEdit();
    pathEdit->setEnabled(false);
    layout->addRow("Path:", pathEdit);
    
    connect(typeCombo, &QComboBox::currentTextChanged, [pathEdit](const QString &text) {
        pathEdit->setEnabled(text == "file");
    });
    
    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addRow(buttons);
    
    if (dialog.exec() == QDialog::Accepted) {
        SinkConfigData sink;
        sink.type = typeCombo->currentText();
        sink.path = pathEdit->text();
        
        m_config.sinks.append(sink);
        updateSinksList();
        emit configurationChanged();
    }
}

void ConfigDialog::onRemoveSink()
{
    int row = m_sinksList->currentRow();
    if (row >= 0 && row < m_config.sinks.size()) {
        m_config.sinks.removeAt(row);
        updateSinksList();
        emit configurationChanged();
    }
}

void ConfigDialog::onEditSink()
{
    onSinkDoubleClicked();
}

void ConfigDialog::onSinkDoubleClicked()
{
    int row = m_sinksList->currentRow();
    if (row < 0 || row >= m_config.sinks.size()) return;
    
    SinkConfigData &sink = m_config.sinks[row];
    
    QDialog dialog(this);
    dialog.setWindowTitle("Edit Sink");
    
    QFormLayout *layout = new QFormLayout(&dialog);
    
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItems({"console", "file"});
    typeCombo->setCurrentText(sink.type);
    layout->addRow("Type:", typeCombo);
    
    QLineEdit *pathEdit = new QLineEdit(sink.path);
    pathEdit->setEnabled(sink.type == "file");
    layout->addRow("Path:", pathEdit);
    
    connect(typeCombo, &QComboBox::currentTextChanged, [pathEdit](const QString &text) {
        pathEdit->setEnabled(text == "file");
    });
    
    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addRow(buttons);
    
    if (dialog.exec() == QDialog::Accepted) {
        sink.type = typeCombo->currentText();
        sink.path = pathEdit->text();
        
        updateSinksList();
        emit configurationChanged();
    }
}

void ConfigDialog::onBrowseFilePath()
{
    QString path = QFileDialog::getOpenFileName(this, "Select File");
    if (!path.isEmpty()) {
        // Update the appropriate path field
    }
}

void ConfigDialog::onAccepted()
{
    if (validateConfig()) {
        accept();
    }
}

bool ConfigDialog::validateConfig()
{
    if (m_appNameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Validation Error", 
                            "Application name cannot be empty.");
        return false;
    }
    
    if (m_config.sources.isEmpty()) {
        QMessageBox::warning(this, "Validation Error",
                            "At least one source must be configured.");
        return false;
    }
    
    if (m_config.sinks.isEmpty()) {
        QMessageBox::warning(this, "Validation Error",
                            "At least one sink must be configured.");
        return false;
    }
    
    return true;
}

bool ConfigDialog::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (doc.isNull()) {
        return false;
    }
    
    QJsonObject root = doc.object();
    
    // Parse application settings
    QJsonObject appObj = root["application"].toObject();
    m_config.appName = appObj["name"].toString("TelemetryApp");
    
    // Parse sources
    m_config.sources.clear();
    QJsonArray sourcesArray = root["sources"].toArray();
    for (const QJsonValue &val : sourcesArray) {
        QJsonObject obj = val.toObject();
        SourceConfigData source;
        source.type = obj["type"].toString();
        source.path = obj["path"].toString();
        source.telemetryType = obj["telemetryType"].toString();
        source.rateMs = obj["rateMs"].toInt(500);
        m_config.sources.append(source);
    }
    
    // Parse sinks
    m_config.sinks.clear();
    QJsonArray sinksArray = root["sinks"].toArray();
    for (const QJsonValue &val : sinksArray) {
        QJsonObject obj = val.toObject();
        SinkConfigData sink;
        sink.type = obj["type"].toString();
        sink.path = obj["path"].toString();
        m_config.sinks.append(sink);
    }
    
    setConfig(m_config);
    return true;
}

bool ConfigDialog::saveToFile(const QString &filePath)
{
    AppConfigData config = getConfig();
    
    QJsonObject root;
    
    // Application settings
    QJsonObject appObj;
    appObj["name"] = config.appName;
    root["application"] = appObj;
    
    // Sources
    QJsonArray sourcesArray;
    for (const SourceConfigData &source : config.sources) {
        QJsonObject obj;
        obj["type"] = source.type;
        obj["path"] = source.path;
        obj["telemetryType"] = source.telemetryType;
        obj["rateMs"] = source.rateMs;
        sourcesArray.append(obj);
    }
    root["sources"] = sourcesArray;
    
    // Sinks
    QJsonArray sinksArray;
    for (const SinkConfigData &sink : config.sinks) {
        QJsonObject obj;
        obj["type"] = sink.type;
        obj["path"] = sink.path;
        sinksArray.append(obj);
    }
    root["sinks"] = sinksArray;
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}