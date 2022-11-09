//
// Created by AlexKDR on 22.10.2022.
//

#include "main_window.h"
#include <algorithm>
#include <qapplication.h>
#include <qaction.h>
#include <qdockwidget.h>
#include <qjsondocument.h>
#include <qstackedwidget.h>
#include <qstatusbar.h>
#include <qboxlayout.h>
#include <qbuttongroup.h>
#include <qevent.h>
#include <qjsonarray.h>
#include <qicon.h>
#include <qmenu.h>
#include <qtcpsocket.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <QtWidgets/QDialogButtonBox>
#include <QtGui/QRegExpValidator>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QMessageBox>
#include "qfiledialog.h"
#include "functional"

MainWindow::MainWindow(int argv, char** argc, QWidget *parent)
        : QMainWindow(parent)
        , CentralWin(new QWidget(parent))
        , Toolbar(nullptr)
        , serverLabel(new QLabel("Server not connected", this))
        , dbLabel(new QLabel("DB not connected", this))
        , TableView(new QTableView(this))
        , logWidget(new QListWidget(this))
{
    openFileLoadConfig();
    paramService = new ParamService(ConfJson, [this]() {paramServiceModel->update();});
    settingsDlg = new Settings_dlg(&Socket, paramService->getDbDriver(), ConfJson, this);
    paramServiceModel = new ParamService_model(paramService->getPtrList(), this);

    Toolbar = CreateToolbar();
    auto* mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(10, 10, 10, 10);
    CentralWin->setLayout(mainLayout);
    setCentralWidget(CentralWin);
    makeStatusLabel();
    mainLayout->addWidget(makeParamTable());
    mainLayout->addWidget(makeParamSetGroupBox());
    mainLayout->addWidget(logWidgetConfig());

    Socket.AddTxMsgHandler([this](const ProtosMessage& txMsg) { txMsgHandler(txMsg);});
    connect(TableView, SIGNAL(clicked(const QModelIndex&)),this, SLOT(OnClickedTableCell(const QModelIndex&)));

    checkServicesConnection();
//    updateTimerID = startTimer(300);
}

QGroupBox* MainWindow::makeParamSetGroupBox(){
    auto* addParamGroupBox = new QGroupBox(tr("Some settings here"), this);
    addParamGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto* searchGroupBoxLayout = new QHBoxLayout(addParamGroupBox);
    addParamGroupBox->setLayout(searchGroupBoxLayout);
    auto* makeEvent = new QPushButton("Make Event");
    connect(makeEvent, &QPushButton::clicked, [this]()
    {
        qDebug()<<"clicked";
    });
    auto* addParamBtn = new QPushButton("Add Param");
    connect(addParamBtn, &QPushButton::clicked, [this]()
    {
        addParam();
    });

    auto* logToFile = new QCheckBox("Log to file");
    connect(logToFile, &QCheckBox::toggled, [this]()
    {
        logToFileFlag = !logToFileFlag;
    });

    makeEvent->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    makeEvent->setEnabled(true);
    addParamBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    addParamBtn->setEnabled(true);
    searchGroupBoxLayout->addWidget(addParamBtn);
    searchGroupBoxLayout->addWidget(makeEvent);
    searchGroupBoxLayout->addWidget(logToFile);
    return addParamGroupBox;
}

void MainWindow::makeStatusLabel(){
    serverLabel->setAlignment(Qt::AlignHCenter);
    serverLabel->setMinimumSize(serverLabel->sizeHint());
    dbLabel->setAlignment(Qt::AlignHCenter);
    dbLabel->setMinimumSize(dbLabel->sizeHint());
    statusBar()->addWidget(serverLabel);
    statusBar()->addWidget(dbLabel);
}

QTableView* MainWindow::makeParamTable(){
    TableView->setModel(paramServiceModel);
    TableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    TableView->setContextMenuPolicy(Qt::CustomContextMenu);
    TableView->setColumnWidth(0, 20);
    TableView->setColumnWidth(1, 50);
    TableView->setColumnWidth(2, 80);
    TableView->setColumnWidth(3, 60);
    TableView->setColumnWidth(4, 60);
    TableView->setColumnWidth(5, 110);
    TableView->setColumnWidth(6, 80);
    TableView->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    return TableView;
}

void MainWindow::saveAll(){
    paramService->saveParams();
    QJsonDocument doc;
    doc.setObject(ConfJson);
    if(!configFile->isOpen()) {
        configFile = new QFile(QCoreApplication::applicationDirPath() + "/../saved_params.json");
        configFile->open(QIODevice::ReadWrite);
    }
    configFile->resize(0);
    configFile->write(doc.toJson(QJsonDocument::Indented));
    configFile->close();
}

QToolBar* MainWindow::CreateToolbar()
{
    QToolBar* toolbar = addToolBar(QString());
    toolbar->setMovable(false);

    QFile styleFile(QCoreApplication::applicationDirPath() + "/../qss/toolbar.css");
    styleFile.open(QIODevice::ReadOnly);
    toolbar->setStyleSheet(styleFile.readAll());
    styleFile.close();

    auto settingsWindow = toolbar->addAction(QString("Settings"));
    settingsWindow->setObjectName("Settings");
    settingsWindow->setToolTip(QString("Open settings window"));
    connect(settingsWindow, &QAction::triggered, [this](bool checked)
    {
        settingsDlg->show();
        settingsDlg->raise();
        settingsDlg->setAttribute(Qt::WA_DeleteOnClose);
        settingsDlg->activateWindow();
    });

    auto* expander = new QWidget(toolbar);
    expander->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toolbar->addWidget(expander);

    auto exit = toolbar->addAction( QString("Save & Exit"));
    exit->setToolTip(QString("Save all and close app"));
    connect(exit, &QAction::triggered, [this](bool checked)
    {
        saveAll();
        close();
    });

    return toolbar;
}

void MainWindow::addParam() {
    QDialog dlg(this);
    dlg.setWindowTitle("Add Param");
    auto *layout = new QFormLayout();
    auto label = new QLabel("Add Param", &dlg);
    auto ID = new QLineEdit("");
    auto Note = new QLineEdit("");
    auto DB = new QCheckBox("Write to DB");
    auto *btn_box = new QDialogButtonBox(&dlg);
    btn_box->setStandardButtons(QDialogButtonBox::Ok);
    connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    label->setAlignment(Qt::AlignHCenter);
    layout->addRow(label);
    layout->addRow(tr("ID: "),ID);
    layout->addRow(tr("Note: "),Note);
    layout->addRow(DB);
    layout->addWidget(btn_box);
    dlg.setLayout(layout);
    if(dlg.exec() == QDialog::Accepted) {
        int paramID = ID->text().toInt();
        if(paramID <= 0 || paramID > 255){
            QMessageBox::information(&dlg, "Wrong ID", "ID should be (uint8) > 0 && <= 255");
            return;
        }
        QString paramNote = Note->text();
        bool paramInserted = paramService->addParam(paramID, paramNote, DB->isChecked());
        if(paramInserted) paramServiceModel->addRow();
    }else
        serverLabel->setText(tr("Aborted adding param"));
}

void MainWindow::openFileLoadConfig() {
    configFile = new QFile(QCoreApplication::applicationDirPath() + "/../saved_params.json");
    configFile->open(QIODevice::ReadWrite);
    QByteArray saveData = configFile->readAll();
    QJsonDocument jsonDocument(QJsonDocument::fromJson(saveData));
    ConfJson = jsonDocument.object();
}

void MainWindow::txMsgHandler(const ProtosMessage &txMsg) {
    paramService->ParseBootMsg(txMsg);
}

void MainWindow::timerEvent(QTimerEvent *event){
    paramServiceModel->update();
}

void MainWindow::OnClickedTableCell(const QModelIndex& index) {
    if(paramServiceModel->isDeleteCellClicked(index)){
        int deleteID = paramServiceModel->getParamID(index);
        QDialog dlg(this);
        dlg.setWindowTitle("Remove Param");
        auto *layout = new QFormLayout();
        auto label = new QLabel(QString("Remove Param: ") + QString("%1?").arg(deleteID,2,16,QChar('0')).toUpper().prepend("0x"), &dlg);
        auto *btn_box = new QDialogButtonBox(&dlg);
        btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::No);
        connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        label->setAlignment(Qt::AlignHCenter);
        layout->addRow(label);
        layout->addWidget(btn_box);
        dlg.setLayout(layout);
        auto exitCode = dlg.exec();
        if(exitCode == QDialog::Accepted) {
            bool paramInserted = paramService->removeParam(deleteID);
            if(paramInserted) paramServiceModel->removeRow(index);
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Protos Config",
                                                                tr("Save params and config?\n"),
                                                                QMessageBox::No | QMessageBox::Save,
                                                                QMessageBox::Save);
    if (resBtn == QMessageBox::Save)
        saveAll();
    else
        close();
}

void MainWindow::checkServicesConnection(){
    if (Socket.IsConnected()){
        serverLabel->setText(tr("Connected to Server"));
        serverLabel->setStyleSheet("color : green");
        AddToLog("Connected to Server");
    }
    else {
        serverLabel->setText(tr("Cant connect to Server"));
        serverLabel->setStyleSheet("color : red");
        AddToLog("Cant connect to Server", true);
    }
    if(paramService->getDbDriver().isConnected()){
        dbLabel->setText(tr("Connected to DB"));
        dbLabel->setStyleSheet("color : green");
        AddToLog("Connected to DB");
    } else{
        dbLabel->setText(tr("Cant connect to DB"));
        dbLabel->setStyleSheet("color : red");
        AddToLog("Cant connect to DB", true);

    }
}

QListWidget* MainWindow::logWidgetConfig(){
    QFile styleFile(QCoreApplication::applicationDirPath() + "/../qss/logWidget.css");
    styleFile.open(QIODevice::ReadOnly);
    logWidget->setStyleSheet(styleFile.readAll());
    styleFile.close();
//    logWidget->setMinimumHeight(5);
    logWidget->setFixedHeight(80);
    logWidget->setAlternatingRowColors(true);
    return logWidget;
}

void MainWindow::AddToLog(const QString& string, bool isError)
{
    logWidget->addItem(QTime::currentTime().toString("HH:mm:ss:zzz").append(" : ").append(string));
    if (isError)
        logWidget->item(logWidget->count() - 1)->setForeground(QBrush(QColor("red")));
}