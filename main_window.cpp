//
// Created by AlexKDR on 22.10.2022.
//

#include "main_window.h"
#include <algorithm>
#include <qapplication.h>
#include <qaction.h>
#include <qdockwidget.h>
#include <qstatusbar.h>
#include <qevent.h>
#include <qmenu.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QMessageBox>
#include "SetParamValueDlg.h"

#ifdef _BUILD_TYPE_
#define CURRENT_BUILD_TYPE_ _BUILD_TYPE_
#else
#define CURRENT_BUILD_TYPE_ "CHECK CMAKE"
#endif

MainWindow::MainWindow(int argv, char** argc, QWidget *parent)
        : QMainWindow(parent)
        , CentralWin(new QWidget(parent))
        , Toolbar(CreateToolbar())
        , serverLabel(new QLabel("ProtosServer", this))
        , dbLabel(new QLabel("DataBase", this))
        , logWidget(new QListWidget(this))
        , tableTabWidget(new QTabWidget(this))
{
    openFileLoadConfig();
    paramService = new ParamService(ConfJson);
    updateParamsTab = new UpdateParamsTab(paramService,this);
    setParamsTab = new SetParamsTab(paramService,this);
    settingsDlg = new Settings_dlg(paramService, ConfJson, this);

    loadMainWindowSettings();
    auto* mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(10, 10, 10, 10);
    CentralWin->setLayout(mainLayout);
    setCentralWidget(CentralWin);
    makeStatusBar();

    mainLayout->addWidget(makeParamSetGroupBox());
    mainLayout->addWidget(makeSplitter());

    checkServicesConnection();

    connect(updateParamsTab, SIGNAL(ParamContextMenuReq(const QModelIndex&, IParamModel::ContextMenuReq)), this, SLOT(OnParamContextMenuReq(const QModelIndex&, IParamModel::ContextMenuReq)));
    connect(updateParamsTab, SIGNAL(tableCellClicked(const QModelIndex&)), this, SLOT(OnClickedTableCell(const QModelIndex&)));
    connect(setParamsTab, SIGNAL(tableCellClicked(const QModelIndex&)), this, SLOT(OnClickedTableCell(const QModelIndex&)));
    connect(setParamsTab, SIGNAL(ParamContextMenuReq(const QModelIndex&, IParamModel::ContextMenuReq)), this, SLOT(OnParamContextMenuReq(const QModelIndex&, IParamModel::ContextMenuReq)));

    connect(paramService, &ParamService::changedParamState, [this](ParamItemType type){
        if(type == UPDATE) updateParamsTab->getModel()->update(IParamModel::UPDATE_TASK);
        else if(type == CONTROL) setParamsTab->getModel()->update(IParamModel::UPDATE_TASK);
    });
    connect(paramService, &ParamService::addedParamFromLine, [this](ParamItemType type){
        updateHostsSet();
        if(type == UPDATE) updateParamsTab->getModel()->update(IParamModel::INSERT_ROW_TASK);
        else if(type == CONTROL) setParamsTab->getModel()->update(IParamModel::INSERT_ROW_TASK);
    });
    connect(paramService, &ParamService::needToResetModels, [this](){
        for(auto it=setParamValueDlgMap.begin(); it!=setParamValueDlgMap.end(); it++){
            it.value()->setAttribute(Qt::WA_DeleteOnClose);
            it.value()->close();
        }
        setParamValueDlgMap.clear();
        updateHostsSet();
        updateParamsTab->getModel()->update(IParamModel::RESET_TASK);
        setParamsTab->getModel()->update(IParamModel::RESET_TASK);
    });
    connect(paramService, &ParamService::databaseWriteFail, [this](){
        logToFile->setChecked(paramService->isWriteToFile());
        AddToLog(QString("Failed to write param in DataBase - please reconnect in setting and uncheck logToFile if needed to return in normal mode \n Log to file now is on! "),true);
        checkServicesConnection();
    });
    connect(paramService, &ParamService::errorInDBToLog, [this](const QString& errorStr){
        AddToLog(errorStr,true);
        checkServicesConnection();
    });
    connect(paramService, &ParamService::eventInDBToLog, [this](const QString& eventStr){ AddToLog(eventStr);});
    connect(settingsDlg, &Settings_dlg::eventInSettings, [this](const QString& str, bool err){
        AddToLog(str, err);
        checkServicesConnection();
    });
    connect(settingsDlg, &Settings_dlg::updateNOfReConnections, [this](int n){
        serverLabel->setText(QString("ProtosServer %1").arg(n));
    });
}

QSplitter* MainWindow::makeSplitter(){
    auto* pSplitter = new QSplitter(CentralWin);
    pSplitter->setSizePolicy(QSizePolicy::QSizePolicy::Expanding,
                             QSizePolicy::QSizePolicy::Expanding);
    pSplitter->setOrientation(Qt::Vertical);
    pSplitter->addWidget(makeTabs());
    pSplitter->addWidget(logWidgetConfig());
    pSplitter->setStretchFactor(0, 1);
    pSplitter->setStretchFactor(1, 0);
    pSplitter->setStyleSheet("QSplitter::handle { background-color: lightgray; }");
    return pSplitter;
}

QGroupBox* MainWindow::makeParamSetGroupBox(){
    auto* addParamGroupBox = new QGroupBox(tr(""), this);
    addParamGroupBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Minimum,QSizePolicy::ButtonBox));

    auto* searchGroupBoxLayout = new QHBoxLayout(addParamGroupBox);
    searchGroupBoxLayout->setSpacing(10);
    addParamGroupBox->setLayout(searchGroupBoxLayout);

    auto* makeEvent = new QPushButton("Make Event");
    connect(makeEvent, &QPushButton::clicked, [this]()
    {
        QDialog dlg(this);
        dlg.setWindowTitle("Make Event");
        auto eventStr = new QLineEdit("");
        auto *btn_box = new QDialogButtonBox(&dlg);
        btn_box->setStandardButtons(QDialogButtonBox::Save);
        connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        auto *layout = new QFormLayout();
        layout->addRow(tr("Event String: "), eventStr);
        layout->addWidget(btn_box);
        dlg.setLayout(layout);
        if(dlg.exec() == QDialog::Accepted)
            paramService->logEventToDB(eventStr->text());
    });
    logToFile = new QCheckBox("Log to file");
    logToFile->setChecked(paramService->isWriteToFile());
    connect(logToFile, &QCheckBox::toggled, [this](bool checked)
    {
        paramService->setWriteToFile(checked);
    });

    auto sortByHostLabel = new QLabel("Sort all params by host 0x", this);
    sortByHostLabel->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed,QSizePolicy::Label));
    hostSortCombBox = new QComboBox(this);
    hostSortCombBox->setEditable(false);
    updateHostsSet();
    connect(hostSortCombBox, &QComboBox::currentTextChanged, [this](const QString& selectedHost){

    });
    connect(hostSortCombBox, &QComboBox::currentTextChanged, [this](const QString& selectedHost){
        paramService->sortAllParamsAboutHost(selectedHost);
        setParamsTab->getModel()->update(IParamModel::RESET_TASK);
        updateParamsTab->getModel()->update(IParamModel::RESET_TASK);
    });

    makeEvent->setEnabled(true);
    searchGroupBoxLayout->addWidget(makeEvent);
    searchGroupBoxLayout->addWidget(logToFile);
    searchGroupBoxLayout->addWidget(sortByHostLabel);
    searchGroupBoxLayout->addWidget(hostSortCombBox);
    return addParamGroupBox;
}

void MainWindow::updateHostsSet(){
    allHostsSet = paramService->getAllHostsInSet();
    hostSortCombBox->clear();
    for(auto& host: allHostsSet)
        hostSortCombBox->addItem(QString("%1").arg(host,0,16));
    hostSortCombBox->insertItem( 0,"Show all");
    hostSortCombBox->setCurrentIndex(0);
}

void MainWindow::makeStatusBar(){
    serverLabel->setAlignment(Qt::AlignHCenter);
    serverLabel->setMinimumSize(serverLabel->sizeHint());
    dbLabel->setAlignment(Qt::AlignHCenter);
    dbLabel->setMinimumSize(dbLabel->sizeHint());
    statusBar()->addWidget(serverLabel);
    statusBar()->addWidget(dbLabel);
}

QTabWidget* MainWindow::makeTabs(){
    tableTabWidget->setTabShape(QTabWidget::Rounded);
    tableTabWidget->setTabPosition(QTabWidget::North);
    tableTabWidget->setMovable(true);
    tableTabWidget->setTabBarAutoHide(true);
    tableTabWidget->addTab(updateParamsTab, "Params");
//    tableTabWidget->addTab(updateParamsTab, "Update Params");
//    tableTabWidget->addTab(setParamsTab, "Control Params");
    return tableTabWidget;
}

QToolBar* MainWindow::CreateToolbar()
{
    QToolBar* toolbar = addToolBar(QString());
    toolbar->setMovable(false);

    auto settingsWindow = toolbar->addAction(QString("Settings"));
    settingsWindow->setObjectName("Settings");
    settingsWindow->setToolTip(QString("Open settings window"));
    connect(settingsWindow, &QAction::triggered, [this]()
    {
        settingsDlg->show();
        settingsDlg->raise();
        settingsDlg->activateWindow();
    });

    auto* expander = new QWidget(toolbar);
    expander->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toolbar->addWidget(expander);

    auto exit = toolbar->addAction( QString("Save"));
    exit->setToolTip(QString("Save all and close app"));
    connect(exit, &QAction::triggered, [this](bool checked)
    {
        saveAll();
    });

    return toolbar;
}

QListWidget* MainWindow::logWidgetConfig(){
    logWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    logWidget->setSizePolicy(QSizePolicy::QSizePolicy::Expanding,
                                 QSizePolicy::QSizePolicy::Expanding);
    logWidget->setAlternatingRowColors(true);
    return logWidget;
}

void MainWindow::openFileLoadConfig() {
    auto pathToFile = QString(CURRENT_BUILD_TYPE_) == "Debug" ? "/../" : "/";
    configFile = new QFile(QCoreApplication::applicationDirPath() + QString("%1/saved_params.json").arg(pathToFile));
    configFile->open(QIODevice::ReadWrite);
    QByteArray saveData = configFile->readAll();
    QJsonDocument jsonDocument(QJsonDocument::fromJson(saveData));
    ConfJson = jsonDocument.object();

    QFile styleFile(QCoreApplication::applicationDirPath() + QString("%1/qss/logWidget.css").arg(pathToFile));
    styleFile.open(QIODevice::ReadOnly);
    logWidget->setStyleSheet(styleFile.readAll());
    styleFile.close();

    styleFile.setFileName(QCoreApplication::applicationDirPath() + QString("%1/qss/main.css").arg(pathToFile));
    styleFile.open(QIODevice::ReadOnly);
    this->setStyleSheet(styleFile.readAll());
    styleFile.close();

    styleFile.setFileName(QCoreApplication::applicationDirPath() + QString("%1/qss/toolbar.css").arg(pathToFile));
    styleFile.open(QIODevice::ReadOnly);
    Toolbar->setStyleSheet(styleFile.readAll());
    styleFile.close();
}

void MainWindow::timerEvent(QTimerEvent *event){
    updateParamsTab->getModel()->update(IParamModel::UPDATE_TASKS::UPDATE_TASK);
}

void MainWindow::OnClickedTableCell(const QModelIndex &index) {
    auto* model = (IParamService_model*)index.model();
    auto* param = model->getParam(index.row());
    if(param == nullptr) return;
    auto mapKey = ParamService::makeMapKey(*param);
    if(model->isSetCellClicked(index.column())){
        if(setParamValueDlgMap.contains(mapKey)) {
            setParamValueDlgMap.value(mapKey)->show();
            setParamValueDlgMap.value(mapKey)->raise();
            return;
        }
        auto* dlg = new SetParamValueDlg(param->getParamId(), param->getHostID(), this);
        dlg->show();
        dlg->raise();
        dlg->activateWindow();
        connect(dlg, &SetParamValueDlg::paramValueSet, [this, index](uchar paramID, uchar hostID, const QVariant& value) {
            paramService->setParamValueChanged(index.row(), value);
        });
        setParamValueDlgMap.insert(mapKey, dlg);
    }
}

void MainWindow::saveMainWindowSettings(){
    QJsonObject confObj;
    confObj["SelfAddr"] = paramService->getSelfAddr();
    confObj["WriteToFile"] = paramService->isWriteToFile();
    ConfJson["MainWindow_Conf"] = confObj;
}

void MainWindow::loadMainWindowSettings(){
    auto confObject = ConfJson.value("MainWindow_Conf");
    paramService->setSelfAddr(confObject["SelfAddr"].toInt());
//    paramService->setWriteToFile(confObject["WriteToFile"].toBool());//todo check about this
}

void MainWindow::saveAll(){
    paramService->saveParams();
    saveMainWindowSettings();
    QJsonDocument doc;
    doc.setObject(ConfJson);
    if(!configFile->isOpen()) {
        auto pathToFile = QString(CURRENT_BUILD_TYPE_) == "Debug" ? "/../" : "/";
        configFile = new QFile(QCoreApplication::applicationDirPath() + QString("%1/saved_params.json").arg(pathToFile));
        configFile->open(QIODevice::ReadWrite);
    }
    configFile->resize(0);
    configFile->write(doc.toJson(QJsonDocument::Indented));
    configFile->close();
    saveLogToFile();
}

void MainWindow::saveLogToFile(){
    auto fileName = QDateTime::currentDateTime().toString(QString("yyyy.MM.dd_hh.mm.ss")).append(".txt");
    auto pathToLogs = QString(CURRENT_BUILD_TYPE_) == "Debug" ? "/../appLogs" : "/appLogs";
    auto logFile = new QFile(QCoreApplication::applicationDirPath() + QString("%1/%2").arg(pathToLogs).arg(fileName));
    logFile->open(QIODevice::ReadWrite);
    QTextStream textStream;
    textStream.setDevice(logFile);
    for(int i=0;i<logWidget->count()-1;i++)
        textStream << logWidget->item(i)->text() + '\n';
    textStream.flush();
    logFile->close();
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Protos Config",
                                                                tr("Save params and config?\n"),
                                                                QMessageBox::No | QMessageBox::Save,
                                                                QMessageBox::Save);
    if (resBtn == QMessageBox::Save)
        saveAll();
    paramService->closeAll();
    configFile->close();
    for(auto it=setParamValueDlgMap.begin(); it!=setParamValueDlgMap.end(); it++){
        it.value()->setAttribute(Qt::WA_DeleteOnClose);
        it.value()->close();
    }
    close();
}

void MainWindow::checkServicesConnection(){
    if(paramService->isSocketConnected()){
        serverLabel->setText("ProtosServer");
        serverLabel->setStyleSheet("color : green");
    }else
        serverLabel->setStyleSheet("color : red");

    updateServiceLabel(dbLabel, paramService->isDataBaseOk());
}

inline void MainWindow::updateServiceLabel(QLabel* label, bool status){
    status ? label->setStyleSheet("color : green") : label->setStyleSheet("color : red");
}

void MainWindow::AddToLog(const QString& string, bool isError)
{
    if(string.isEmpty()) return;
    logWidget->addItem(QTime::currentTime().toString("HH:mm:ss:zzz").append(" : ").append(string));
    logWidget->item(logWidget->count() - 1)->
            setForeground(isError ? QBrush(QColor("red")) : QBrush(QColor("green")));
}

void MainWindow::sortColumns(ParamItemType type, IParamModel::Headers header){
    if(header == IParamModel::PARAM_ID) paramService->sortByParamID(type);
    else if(header == IParamModel::PARAM_HOST) paramService->sortByHostID(type);
    if(type == UPDATE)
        updateParamsTab->getModel()->update(IParamModel::RESET_TASK);
    else if(type == CONTROL)
        setParamsTab->getModel()->update(IParamModel::RESET_TASK);
}

void MainWindow::OnParamContextMenuReq(const QModelIndex &index, IParamModel::ContextMenuReq req) {
    auto* model = (IParamService_model*)index.model();
    auto* param = model->getParam(index.row());
    if(param == nullptr) return;
    auto mapKey = ParamService::makeMapKey(*param);
    switch (req){
        case IParamModel::Config:
            paramService->configParam(mapKey);
            break;
        case IParamModel::Move:
//            paramService->moveUpdateToSet(mapKey);
            break;
        case IParamModel::Delete:
            bool paramDeleted = paramService->removeParam(mapKey);
            if(paramDeleted) model->removeRow(index);
            if(setParamValueDlgMap.contains(mapKey)){
                setParamValueDlgMap[mapKey]->setAttribute(Qt::WA_DeleteOnClose);
                setParamValueDlgMap[mapKey]->close();
                setParamValueDlgMap.remove(mapKey);
            }
            updateHostsSet();
            break;
    }
}
