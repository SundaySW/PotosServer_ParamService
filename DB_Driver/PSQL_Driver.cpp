//
// Created by AlexKDR on 23.10.2022.
//

#include "PSQL_Driver.h"
#include "QSqlQuery"
#include "QSqlError"

PSQL_Driver::PSQL_Driver(QJsonObject& conf)
        :config(conf)
{
    setConnection();
}

bool PSQL_Driver::setConnection() {
    bool result;
    db = QSqlDatabase::addDatabase("QPSQL");
    configUpdate();
    db.setHostName(host);
    db.setDatabaseName(dbName);
    db.setUserName(userName);
    db.setPassword(password);
    db.setPort(port.toInt());
    result = db.open();
    hasConnection = result;
    qDebug()<<"Connected to DB? - " << result;
    bool check = false;
    check = createTable("FirstTable");
    qDebug()<<"Is table created? - " << check;

    return result;
}

bool PSQL_Driver::createTable(const QString& tableName){
    QSqlQuery query;
    if(!query.exec("CREATE TABLE accounts("
                   "user_id serial PRIMARY KEY,"
                   "username VARCHAR ( 50 ) UNIQUE NOT NULL,"
                   "password VARCHAR ( 50 ) NOT NULL,"
                   "email VARCHAR ( 255 ) UNIQUE NOT NULL"
                   ");"))
    {
        qDebug() << QString("DataBase: error of create %1. Error: %2").arg(tableName).arg(query.lastError().text());
        return false;
    }else
        return true;
}

bool PSQL_Driver::hasTable(const QString& tableName){

}

bool PSQL_Driver::isConnected() const {
    return db.isOpen();
}

void PSQL_Driver::writeParam(ParamItem) {

}

void PSQL_Driver::configUpdate() {
    auto confObject = config.value("DBConfObject");
    host = confObject["HostName"].toString();
    dbName = confObject["DatabaseName"].toString();
    userName = confObject["UserName"].toString();
    password = confObject["Password"].toString();
    port = confObject["Port"].toString();
    autoConnect = confObject["autoconnect"].toBool();
}
