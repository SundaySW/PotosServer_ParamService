//
// Created by AlexKDR on 23.10.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_PSQL_DRIVER_H
#define POTOSSERVER_PARAMSERVICE_PSQL_DRIVER_H

#include <QSqlDatabase>
#include <Param_Item/ParamItem.h>

class PSQL_Driver {
public:
    PSQL_Driver(QJsonObject &conf);
    bool setConnection();
    void writeParam(ParamItem);
    bool isConnected() const;
    void configUpdate();
private:
    QSqlDatabase db;
    bool hasConnection;
    QJsonObject& config;
    QString host, dbName, userName, password, port;
    bool autoConnect;

    bool createTable(const QString &tableName);

    bool hasTable(const QString &tableName);
};


#endif //POTOSSERVER_PARAMSERVICE_PSQL_DRIVER_H
