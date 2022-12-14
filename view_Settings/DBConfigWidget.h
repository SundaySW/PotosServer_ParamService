//
// Created by outlaw on 02.11.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_DBCONFIGWIDGET_H
#define POTOSSERVER_PARAMSERVICE_DBCONFIGWIDGET_H

#include <QtWidgets/QWidget>
#include <Monitor/socket_adapter.h>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include "QFormLayout"
#include <QtWidgets/QLabel>
#include <QtSql/QSqlDatabase>
#include <DB_Driver/PSQL_Driver.h>
#include "QJsonObject"
#include "QCheckBox"

class DBConfigWidget : public QWidget
{
    Q_OBJECT
public:
    DBConfigWidget(QJsonObject &JsonConf, PSQL_Driver& databaseDriver);
    virtual	~DBConfigWidget(){};
    void Save();
    void Set();
signals:
    void eventInDBConnection(const QString&, bool);
private:
    QLineEdit* HostName, *DatabaseName, *UserName, *Password, *Port;
    QCheckBox* autoconnect;
    QPushButton* connectBtn;
    QPushButton* saveBtn;
    QLabel* statusLabel;
    QJsonObject& savedConf;
    PSQL_Driver& dbDriver;
    void setEditsStateDisabled(bool state);
    void updateView();
};


#endif //POTOSSERVER_PARAMSERVICE_DBCONFIGWIDGET_H
