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
#define MAX_COUNT_OF_RECONNECT  3
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
    void updateCountOfReConnections(int count);
private:
    SocketAdapter& Socket;
    QLineEdit* AddrEdit, *PortEdit;
    QCheckBox* autoReConnect;
    QPushButton* connectBtn;
    QPushButton* saveBtn;
    QLabel* statusLabel;
    QJsonObject& savedConf;
    QTimer* checkConnectTimer;
    int countOfReconnect = 0;
    bool connectToSocket(bool fromEnter = false);
    void setEditsStateDisabled(bool state);
    void updateView();
    void checkConnectTimerFinished();
    void startCheckConnectionTimer();
};


#endif //POTOSSERVER_PARAMSERVICE_SERVERCONNECTWIDGET_H
