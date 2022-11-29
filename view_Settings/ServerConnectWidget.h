//
// Created by outlaw on 01.11.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_SERVERCONNECTWIDGET_H
#define POTOSSERVER_PARAMSERVICE_SERVERCONNECTWIDGET_H

#include <QtWidgets/QWidget>
#include <Monitor/socket_adapter.h>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include "QFormLayout"
#include <QtWidgets/QLabel>
#include "QJsonObject"
#include "QCheckBox"

class ServerConnectWidget : public QWidget
{
    Q_OBJECT
public:
    ServerConnectWidget(SocketAdapter& socket, QJsonObject& JsonConf);
    ~ServerConnectWidget(){}
    void Set();
    void Save();
signals:
    void eventInServerConnection(const QString&, bool);
private:
    SocketAdapter& Socket;
    QLineEdit* AddrEdit, *PortEdit;
    QPushButton* connectBtn;
    QLabel* statusLabel;
    QJsonObject& savedConf;

    bool connectToSocket();
    void setEditsStateDisabled(bool state);
    void updateView();
};


#endif //POTOSSERVER_PARAMSERVICE_SERVERCONNECTWIDGET_H
