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
    , connectBtn(new QPushButton(tr(""), this))
    , Socket(socket)
    , statusLabel(new QLabel("", this))
    , savedConf(JsonConf)
{
    Set();
    connectToSocket();
    auto *layout = new QFormLayout();
    layout->setContentsMargins(30, 30, 30, 30);
    auto label = new QLabel("Server connection", this);
    label->setAlignment(Qt::AlignTop);
    layout->addRow(label);
    layout->addRow(QString("IP"), AddrEdit);
    layout->addRow(QString("Port"),  PortEdit);
    layout->addWidget(connectBtn);
    statusLabel->setAlignment(Qt::AlignHCenter);
    statusLabel->setMinimumSize(statusLabel->sizeHint());
    layout->addWidget(statusLabel);
    updateView();
    connect(connectBtn, &QPushButton::clicked, [this]() {
        if(Socket.IsConnected()) {
            Socket.Disconnect(1000);
            checkConnectTimer->stop();
            emit eventInServerConnection("Disconnected from server by user", false);
        }else{
            Save();
            if(connectToSocket()) startCheckConnectionTimer();
        }
        updateView();
    });
    this->setLayout(layout);
    checkConnectTimer = new QTimer(this);
    connect(checkConnectTimer, &QTimer::timeout, [this]() {
        checkConnectTimerFinished();
    });
    startCheckConnectionTimer();
}

bool ServerConnectWidget::connectToSocket(){
    QString SocketIp   = AddrEdit->text();
    QString SocketPort = PortEdit->text();
    bool isConnected = Socket.Connect(SocketIp, SocketPort.toInt(), 10);
    QString eventStr;
    bool isError = false;
    if(isConnected){
        eventStr = QString("Event is server connection: connected to sever ip:%1 pot:%2").arg(AddrEdit->text()).arg(PortEdit->text());
        countOfReconnect = 0;
    }else
    {
        eventStr = QString("Event is server connection: cant connect to sever ip:%1 pot:%2").arg(AddrEdit->text()).arg(PortEdit->text());
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
}

void ServerConnectWidget::Save() {
    auto str = AddrEdit->text();
    QJsonObject confObj;
    confObj["ServerIP"] = str.isEmpty() ? "127.0.0.5" : str;
    confObj["ServerPort"] = PortEdit->text();
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
        connectBtn->setText("Save and connect");
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
    emit eventInServerConnection(QString("Server found disconnected - now making %1 of %2 attempt to connect").arg(countOfReconnect).arg(MAX_COUNT_OF_RECONNECT), true);
    connectToSocket();
    if(countOfReconnect == MAX_COUNT_OF_RECONNECT) {
        checkConnectTimer->stop();
        emit eventInServerConnection(QString("Cant reconnect, no more auto connections - please use settings dialog to connect to server").arg(countOfReconnect), true);
    }
}
void ServerConnectWidget::startCheckConnectionTimer(){
    countOfReconnect = 0;
    checkConnectTimer->start(CHECK_CONNECTION_TIME);
}