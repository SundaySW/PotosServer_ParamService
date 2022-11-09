//
// Created by outlaw on 28.10.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_PARAMSERVICE_H
#define POTOSSERVER_PARAMSERVICE_PARAMSERVICE_H

#include <DB_Driver/PSQL_Driver.h>
#include <QtCore/QTimer>
#include "QJsonObject"
#include "QJsonArray"
#include "QJsonDocument"

class ParamService: public QObject{
    Q_OBJECT
public:
    explicit ParamService(QJsonObject&, std::function<void()> func);
    ParamService(const ParamService&)=delete;
    ParamService(ParamService&&)=delete;
    void ParseBootMsg(const ProtosMessage &msg);
    QList<ParamItem*>& getPtrList();
    bool addParam(uint incomeID, const QString &incomeNote, bool writeToDB);
    bool removeParam(uint incomeID);
    void updateParamValue(uchar, QString&&);
    void configureTimer(int);
    void saveParams();
    void loadParams();
    PSQL_Driver& getDbDriver();

private slots:
    void writeTimerUpdate();
private:
    void setPtrListFromMap();
    std::function<void()> updateTableViewFunc;
    QMap<uchar, ParamItem> dataMap;
    QList<ParamItem*> ptrList;
    PSQL_Driver dbDriver;
    QTimer* writeTimer;
    int mSecWriteTimer = 1000;
    QJsonObject& qJsonObject;
};

#endif //POTOSSERVER_PARAMSERVICE_PARAMSERVICE_H
