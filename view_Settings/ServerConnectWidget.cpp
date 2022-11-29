//
// Created by outlaw on 01.11.2022.
//

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include "ServerConnectWidget.h"
#include "QJsonObject"

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
        QString eventStr;
        bool isError = false;
        if(Socket.IsConnected())
            Socket.Disconnect(1000);
        else{
            Save();
            if(!connectToSocket()){
                statusLabel->setText("Error on connect to Protos Server");
                statusLabel->setStyleSheet("color:red");
                eventStr = QString("Event is server connection: cant connect to sever ip:%1 pot:%2").arg(AddrEdit->text()).arg(PortEdit->text());
                isError = true;
                return;
            }else
                eventStr = QString("Event is server connection: connected to sever ip:%1 pot:%2").arg(AddrEdit->text()).arg(PortEdit->text());
        }
        emit eventInServerConnection(eventStr, isError);
        updateView();
    });
    this->setLayout(layout);
}

bool ServerConnectWidget::connectToSocket(){
    QString SocketIp   = AddrEdit->text();
    QString SocketPort = PortEdit->text();
    return Socket.Connect(SocketIp, SocketPort.toInt(), 1000);
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