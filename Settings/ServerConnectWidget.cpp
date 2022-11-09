//
// Created by outlaw on 01.11.2022.
//

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include "ServerConnectWidget.h"
#include "QJsonObject"

ServerConnectWidget::ServerConnectWidget(SocketAdapter* socket, QJsonObject& JsonConf)
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
    if(Socket->IsConnected()) {
        connectBtn->setText("Connected - Save and reconnect");
        statusLabel->setText(tr("Connected"));
        statusLabel->setStyleSheet("color : green");
    }
    else {
        connectBtn->setText("Disconnected - Save and connect");
        statusLabel->setStyleSheet("color : blue");
        statusLabel->setText(tr("Disconnected - press to connect"));
    }

    connect(connectBtn, &QPushButton::clicked, [this]() {
        Save();
        if (Socket->IsConnected())
        {
            Socket->Disconnect(1000);
            statusLabel->setStyleSheet("color : blue");
            statusLabel->setText(tr("Disconnected - press to connect"));
            connectToSocket();
        }
        else
            connectToSocket();
    });
    this->setLayout(layout);
}

bool ServerConnectWidget::connectToSocket(){
    QString SocketIp   = AddrEdit->text();
    QString SocketPort = PortEdit->text();
    if (Socket->Connect(SocketIp, SocketPort.toInt(), 1000))
    {
        connectBtn->setText("Connected - Save and reconnect");
        statusLabel->setText(tr("Connected"));
        statusLabel->setStyleSheet("color : green");
    }
    else
    {
        connectBtn->setText("Disconnected - Save and connect");
        statusLabel->setText(tr("Error on connection"));
        statusLabel->setStyleSheet("color : red");
    }
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