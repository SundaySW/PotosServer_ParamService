//
// Created by outlaw on 28.10.2022.
//

#include <Monitor/protos_message.h>

#include <utility>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include "ParamService.h"

void ParamService::ParseBootMsg(const ProtosMessage& msg) {
    if ((msg.ProtocolType != ProtosMessage::PROTOS) || !msg.Dlc)
        return;
}

QList<ParamItem*>& ParamService::getPtrList() {
    return ptrList;
}

bool ParamService::addParam(uint incomeID, const QString &incomeNote, bool writeToDB) {
    if(dataMap.contains(incomeID)) return false;
    dataMap.insert(incomeID,ParamItem(incomeID, incomeNote, writeToDB));
    setPtrListFromMap();
    return true;
}

bool ParamService::removeParam(uint incomeID) {
    if(!dataMap.contains(incomeID)) return false;
    dataMap.remove(incomeID);
    setPtrListFromMap();
    return true;
}

ParamService::ParamService(QJsonObject& inJson, std::function<void()> func)
    : updateTableViewFunc(std::move(func))
    , qJsonObject(inJson)
    , dbDriver(PSQL_Driver(inJson))
{
    loadParams();
//    configureTimer(mSecWriteTimer);
//    auto data = ParamItem(0x12);
//    data.setValue("333");
//    data.setNote("First Param");
//    data.setWriteToDb(true);
//    data.setLastValueTime(QDateTime::currentDateTime());
//    data.setState(ParamItem::States::InDB);
//
//    auto data2 = ParamItem(0x14);
//    data2.setValue("");
//    data2.setNote("Second Param");
//    data2.setWriteToDb(false);
//    dataMap.insert(data.getId(),data);
//    dataMap.insert(data2.getId(),data2);
//    loadParams();
//    setPtrListFromMap();
}

void ParamService::updateParamValue(uchar paramID, QString&& value) {
    if(dataMap.contains(paramID))
        dataMap[paramID].setValue(value);
    updateTableViewFunc();
}

void ParamService::configureTimer(int timerValue) {
    writeTimer = new QTimer();
    connect(writeTimer, SIGNAL(timeout()), this, SLOT(writeTimerUpdate()));
    writeTimer->start(mSecWriteTimer);
}

void ParamService::writeTimerUpdate() {
    qDebug("dd");
}

inline void ParamService::setPtrListFromMap(){
    ptrList.clear();
    auto it = dataMap.begin();
    auto end = dataMap.end();
    while(it != end){
        ptrList.append(&it.value());
        it++;
    }
}

void ParamService::saveParams() {
    QJsonArray paramArr;
    for(auto& p: dataMap)
        paramArr.append(p.getJsonObject());
    qJsonObject["Params"] = paramArr;
}

void ParamService::loadParams() {
    QJsonArray paramArr = qJsonObject["Params"].toArray();
    dataMap.clear();
    for (const auto& param : paramArr)
    {
        QJsonObject loadParam = param.toObject();
        if (param != QJsonObject())
        {
            ParamItem item(loadParam);
            dataMap.insert(item.getId(), item);
        }
    }
    setPtrListFromMap();
}

PSQL_Driver& ParamService::getDbDriver() {
    return dbDriver;
}
