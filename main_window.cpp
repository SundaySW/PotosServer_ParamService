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
    updateParamsTab = new UpdateParamsTab(paramService->getPtrList(ParamItemType::UPDATE),this);
    setParamsTab = new SetParamsTab(paramService->getPtrList(ParamItemType::SET),this);
    settingsDlg = new Settings_dlg(paramService, ConfJson, this);

    loadMainWindowSettings();
    auto* mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(10, 10, 10, 10);
    CentralWin->setLayout(mainLayout);
    setCentralWidget(CentralWin);

    makeStatusBar();
    mainLayout->addWidget(makeTabs());
    mainLayout->addWidget(makeParamSetGroupBox());
    mainLayout->addWidget(logWidgetConfig());

    connect(updateParamsTab, SIGNAL(tableCellClicked(const QModelIndex&)), this, SLOT(OnClickedTableCell(const QModelIndex&)));
    connect(updateParamsTab, &UpdateParamsTab::addedParamSig, [this](uchar incomeID, uchar incomeHostAddr, ParamItemType incomeType){
        paramService->addParam(incomeID, incomeHostAddr, incomeType);
    });
    connect(updateParamsTab, SIGNAL(onlyDBParamShow(bool)), this, SLOT(OnlyInDBShow(bool)));
    connect(updateParamsTab, SIGNAL(onlyOnlineParamShow(bool)), this, SLOT(OnlyOnlineShow(bool)));
    connect(updateParamsTab, SIGNAL(tableCellDataUpdated(const QModelIndex&)), this, SLOT(OnChangedValueTableCell(const QModelIndex&)));

    connect(setParamsTab, SIGNAL(tableCellClicked(const QModelIndex&)), this, SLOT(OnClickedTableCell(const QModelIndex&)));
    connect(setParamsTab, &SetParamsTab::addedParamSig, [this](uchar incomeID, uchar incomeHostAddr, ParamItemType incomeType){
        paramService->addParam(incomeID, incomeHostAddr, incomeType);
    });
    connect(setParamsTab, SIGNAL(tableCellDataUpdated(const QModelIndex&)), this, SLOT(OnChangedValueTableCell(const QModelIndex&)));

    connect(paramService, &ParamService::changedParamState, [this](ParamItemType type){
        if(type == UPDATE) updateParamsTab->getModel()->update(IParamService_model::UPDATE_TASK);
        else if(type == SET) setParamsTab->getModel()->update(IParamService_model::UPDATE_TASK);
    });
    connect(paramService, &ParamService::addedParamFromLine, [this](ParamItemType type){
        updateHostsSet();
        if(type == UPDATE) updateParamsTab->getModel()->update(IParamService_model::INSERT_ROW_TASK);
        else if(type == SET) setParamsTab->getModel()->update(IParamService_model::INSERT_ROW_TASK);
    });
    connect(paramService, &ParamService::needToResetModels, [this](){
        updateParamsTab->getModel()->update(IParamService_model::RESET_TASK);
        setParamsTab->getModel()->update(IParamService_model::RESET_TASK);
    });
    connect(paramService, &ParamService::databaseWriteFail, [this](){
        logToFile->setChecked(paramService->isWriteToFile());
        AddToLog(QString("Failed to write param in DataBase - please reconnect in setting and uncheck logToFile if needed to return in normal mode \n Log to file now is on! "),true);
        checkServicesConnection();
    });
    connect(paramService, &ParamService::errorInDBToLog, [this](const QString& errorStr){ AddToLog(errorStr,true);});
    connect(paramService, &ParamService::eventInDBToLog, [this](const QString& eventStr){ AddToLog(eventStr);});
    connect(settingsDlg, &Settings_dlg::eventInSettings, [this](const QString& str, bool err){
        AddToLog(str, err);
        checkServicesConnection();
    });

    connect(settingsDlg, SIGNAL(settingsDialogClosed()), this, SLOT(checkServicesConnection()));
    checkServicesConnection();
//  updateTimerID = startTimer(300);
}

QGroupBox* MainWindow::makeParamSetGroupBox(){
    auto* addParamGroupBox = new QGroupBox(tr("Some settings here"), this);
    addParamGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto* searchGroupBoxLayout = new QHBoxLayout(addParamGroupBox);
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

    auto selfAddrLabel = new QLabel("Self Address: ", this);
    auto selfAddr = new QLineEdit( this);
    selfAddr->setText(QString("%1").arg(paramService->getSelfAddr(),0,16));
    connect(selfAddr, &QLineEdit::textChanged, [this](const QString& newSelfAddr){
        paramService->setSelfAddr(newSelfAddr.toInt(nullptr,16));
    });

    auto sortByHostLabel = new QLabel("Sort all tabs by host 0x: ", this);
    hostSortCombBox = new QComboBox(this);
    hostSortCombBox->setEditable(false);
    updateHostsSet();
    connect(hostSortCombBox, &QComboBox::currentTextChanged, [this](const QString& selectedHost){
        paramService->sortAllParamsAboutHost(selectedHost);
        setParamsTab->getModel()->update(IParamService_model::RESET_TASK);
        updateParamsTab->getModel()->update(IParamService_model::RESET_TASK);
    });

//    makeEvent->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed,QSizePolicy::ToolButton));
//    logToFile->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed,QSizePolicy::ToolButton));
//    selfAddr->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed,QSizePolicy::ToolButton));
//    hostSortCombBox->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed,QSizePolicy::ToolButton));

    makeEvent->setEnabled(true);
    searchGroupBoxLayout->addWidget(makeEvent);
    searchGroupBoxLayout->addWidget(logToFile);
    searchGroupBoxLayout->addWidget(selfAddrLabel);
    searchGroupBoxLayout->addWidget(selfAddr);
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
    tableTabWidget->addTab(updateParamsTab, "Update Params");
    tableTabWidget->addTab(setParamsTab, "Control Params");
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
    logWidget->setFixedHeight(80);
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

    styleFile.setFileName(QCoreApplication::applicationDirPath() + QString("%1/qss/toolbar.css").arg(pathToFile));
    styleFile.open(QIODevice::ReadOnly);
    Toolbar->setStyleSheet(styleFile.readAll());
    styleFile.close();
}

void MainWindow::timerEvent(QTimerEvent *event){
    updateParamsTab->getModel()->update(UpdateParamService_model::UPDATE_TASKS::UPDATE_TASK);
}

void MainWindow::OnClickedTableCell(const QModelIndex &index) {
    auto* model = (IParamService_model*)index.model();
    auto* param = model->getParam(index);
    auto mapKey = ParamService::makeMapKey(*param);
    if(model->getType() == SET && model->isSetCellClicked(index.column())){
        if(setParamValueDlgMap.contains(mapKey)) {
            setParamValueDlgMap.value(mapKey)->show();
            return;
        }
        auto* dlg = new SetParamValueDlg(param->geParamId(), param->getHostID(), this);
        dlg->show();
        dlg->raise();
        dlg->activateWindow();
        connect(dlg, &SetParamValueDlg::paramValueSet, [this, index](uchar paramID, uchar hostID, const QVariant& value) {
            paramService->setParamValueChanged(index.row(), value);
        });
        setParamValueDlgMap.insert(mapKey, dlg);
    }
    if(model->isDeleteCellClicked(index)){
        QDialog dlg(this);
        dlg.setWindowTitle("Move/Remove Param");
        auto *layout = new QFormLayout();
        auto label = new QLabel(QString("Move/Remove Param: ") + QString("%1?").arg(mapKey).toUpper().prepend("0x"), &dlg);
        auto* moveBtn = new QPushButton("Move");
        auto* removeBtn = new QPushButton("Remove");
        label->setAlignment(Qt::AlignHCenter);
        layout->addRow(label);
        if(param->getParamType()==UPDATE) layout->addWidget(moveBtn);
        layout->addWidget(removeBtn);
        dlg.setLayout(layout);
        connect(moveBtn, &QPushButton::clicked, [this, mapKey, &dlg]() {
            paramService->moveUpdateToSet(mapKey);
            dlg.close();
        });
        connect(removeBtn, &QPushButton::clicked, [this, param, model, index, mapKey, &dlg](){
            bool paramDeleted = paramService->removeParam(*param);
            if(paramDeleted) model->removeRow(index);
            if(setParamValueDlgMap.contains(mapKey)){
                setParamValueDlgMap[mapKey]->setAttribute(Qt::WA_DeleteOnClose, true);
                setParamValueDlgMap[mapKey]->close();
                setParamValueDlgMap.remove(mapKey);
            }
            updateHostsSet();
            dlg.close();
        });
        dlg.exec();
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
    paramService->setWriteToFile(confObject["WriteToFile"].toBool());
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
    close();
}

void MainWindow::checkServicesConnection(){
    if(paramService->isSocketConnected())
        serverLabel->setStyleSheet("color : green");
    else
        serverLabel->setStyleSheet("color : red");
    if(paramService->getDbDriver().isDBOk())
        dbLabel->setStyleSheet("color : green");
    else
        dbLabel->setStyleSheet("color : red");
}

void MainWindow::AddToLog(const QString& string, bool isError)
{
    if(string.isEmpty()) return;
    logWidget->addItem(QTime::currentTime().toString("HH:mm:ss:zzz").append(" : ").append(string));
    if (isError)
        logWidget->item(logWidget->count() - 1)->setForeground(QBrush(QColor("red")));
    else
        logWidget->item(logWidget->count() - 1)->setForeground(QBrush(QColor("green")));
}

void MainWindow::OnChangedValueTableCell(const QModelIndex& index) {
//    if(index.column() == SetParamService_model::Headers::VALUE)
//        paramService->setParamValueChanged(index.row());
    auto* model = (IParamService_model*)index.model();
    auto* param = model->getParam(index);
    auto tableName = param->getTableName();
    if(index.column() == SetParamService_model::Headers::PARAM_ID || index.column() == UpdateParamService_model::Headers::PARAM_ID){
        paramService->logViewChangesToDB(QString("%1 : new alt_name : %2").arg(tableName).arg(index.model()->data(index).toString()));
    }
    else if(index.column() == SetParamService_model::Headers::NOTES || index.column() == UpdateParamService_model::Headers::NOTES){
        paramService->logViewChangesToDB(QString("%1 : new note : %2").arg(tableName).arg(index.model()->data(index).toString()));
    }
}

void MainWindow::OnlyInDBShow(bool state) {
    paramService->sortUpdateParamListAboutDB(state);
    updateParamsTab->getModel()->update(IParamService_model::RESET_TASK);
}

void MainWindow::OnlyOnlineShow(bool state) {
    paramService->sortUpdateListAboutOnline(state);
    updateParamsTab->getModel()->update(IParamService_model::RESET_TASK);
}