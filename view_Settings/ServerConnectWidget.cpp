//
// Created by outlaw on 01.11.2022.
//

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include "ServerConnectWidget.h"
#include "QJsonObject"
#define CHECK_CONNECTION_TIME   2900
ServerConnectWidget::ServerConnectWidget(SocketAdapter& socket, QJsonObject& JsonConf)
    : AddrEdit(new QLineEdit("127.0.0.5",this))
    , PortEdit(new QLineEdit("3699", this))
    , autoReConnect(new QCheckBox(this))
    , connectBtn(new QPushButton(tr(""), this))
    , saveBtn(new QPushButton(tr("Save"), this))
    , Socket(socket)
    , statusLabel(new QLabel("", this))
    , savedConf(JsonConf)
{
    Set();
    auto confObject = savedConf.value("ConfObject");
    auto *layout = new QFormLayout();
    layout->setContentsMargins(30, 30, 30, 30);
    auto label = new QLabel("Server connection", this);
    label->setStyleSheet("font-weight: bold; padding: 5px; color: grey;");
    label->setAlignment(Qt::AlignTop);
    layout->addRow(label);
    layout->addRow(QString("IP"), AddrEdit);
    layout->addRow(QString("Port"),  PortEdit);
    layout->addRow(QString("Auto Connect"), autoReConnect);
    layout->addWidget(connectBtn);
    layout->addWidget(saveBtn);
    statusLabel->setAlignment(Qt::AlignHCenter);
    statusLabel->setMinimumSize(statusLabel->sizeHint());
    layout->addWidget(statusLabel);
    updateView();
    connect(connectBtn, &QPushButton::clicked, [this, confObject]() {
        if(Socket.IsConnected()) {
            Socket.Disconnect(30);
            checkConnectTimer->stop();
            emit eventInServerConnection("Disconnected from server by user", false);
        }else{
            connectToSocket(true);
            if(autoReConnect->isChecked()) startCheckConnectionTimer();
        }
        updateView();
    });
    connect(saveBtn, &QPushButton::clicked, [this, confObject]() {
        Save();
        if(autoReConnect->isChecked()) startCheckConnectionTimer();
        else checkConnectTimer->stop();
    });
    this->setLayout(layout);
    checkConnectTimer = new QTimer(this);
    connect(checkConnectTimer, &QTimer::timeout, [this]() {
        checkConnectTimerFinished();
    });
    if(confObject["AutoReConnect"].toBool()) startCheckConnectionTimer();
}

bool ServerConnectWidget::connectToSocket(bool fromEnter) {
    auto confObject = savedConf.value("ConfObject");
    QString ip, port;
    if(fromEnter){
        ip = AddrEdit->text();
        port = PortEdit->text();
    } else{
        ip = confObject["ServerIP"].toString();
        port = confObject["ServerPort"].toString();
    }
    bool isConnected = Socket.Connect(ip, port.toInt(), 30);
    QString eventStr;
    bool isError = false;
    if(isConnected){
        eventStr = QString("Event is server connection: connected to sever ip:%1 pot:%2").arg(ip).arg(port);
        countOfReconnect = 0;
    }else
    {
        if(countOfReconnect == 1 || fromEnter)
            eventStr = QString("Event is server connection: cant connect to sever ip:%1 pot:%2").arg(ip).arg(port);
        isError = true;
    }
    updateView();
    emit eventInServerConnection(eventStr, isError);
    return isConnected;
}

void ServerConnectWidget::Set()
{
    auto confObject = savedConf.value("ConfObject");
    AddrEdit->setText(confObject["ServerIP"].toString());
    PortEdit->setText(confObject["ServerPort"].toString());
    autoReConnect->setChecked(confObject["AutoReConnect"].toBool());
}

void ServerConnectWidget::Save() {
    auto str = AddrEdit->text();
    QJsonObject confObj;
    confObj["ServerIP"] = str.isEmpty() ? "127.0.0.5" : str;
    confObj["ServerPort"] = PortEdit->text();
    confObj["AutoReConnect"] = autoReConnect->isChecked();
    savedConf["ConfObject"] = confObj;
}

void ServerConnectWidget::updateView(){
    if(Socket.IsConnected()){
        connectBtn->setText("Disconnect");
        setEditsStateDisabled(true);
        statusLabel->setText("Connected to Protos Server");
        statusLabel->setStyleSheet("color:green");
    }else{
        setEditsStateDisabled(false);
        connectBtn->setText("Connect");
        statusLabel->setText("Disconnected from Protos Server");
        statusLabel->setStyleSheet("color:red");
    }
}

void ServerConnectWidget::setEditsStateDisabled(bool state){
    AddrEdit->setDisabled(state);
    PortEdit->setDisabled(state);
}

void ServerConnectWidget::checkConnectTimerFinished(){
    if(Socket.IsConnected()) return;
    countOfReconnect++;
//    emit eventInDBConnection(QString("Server found disconnected - now making %1 of %2 attempt to connect").arg(countOfReconnect).arg(MAX_COUNT_OF_RECONNECT), true);
//    emit eventInServerConnection(QString("Server found disconnected - now making %1 attempt to connect").arg(countOfReconnect), true);
    emit updateCountOfReConnections(countOfReconnect);
    connectToSocket();
}
void ServerConnectWidget::startCheckConnectionTimer(){
    countOfReconnect = 0;
    checkConnectTimer->start(CHECK_CONNECTION_TIME);
}