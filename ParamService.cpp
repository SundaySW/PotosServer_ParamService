//
// Created by outlaw on 28.10.2022.
//

#include <Monitor/protos_message.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include "ParamService.h"
#include "UpdateParamService_model.h"

#ifdef _BUILD_TYPE_
#define CURRENT_BUILD_TYPE_ _BUILD_TYPE_
#else
#define CURRENT_BUILD_TYPE_ "CHECK CMAKE"
#endif

ParamService::ParamService(QJsonObject &inJson) :
        qJsonObject(inJson),
        dbDriver(PSQL_Driver(inJson)),
        logFile(new QFile())
{
    dbDriver.errorInDBDriver = [this](const QString& errorStr){ errorInDBToLog(errorStr);};
    dbDriver.eventInDBDriver = [this](const QString& eventStr){ eventInDBToLog(eventStr);};
    socketAdapter.AddTxMsgHandler([this](const ProtosMessage& txMsg) { txMsgHandler(txMsg);});
    socketAdapter.AddRxMsgHandler([this](const ProtosMessage& txMsg) { rxMsgHandler(txMsg);});
    dbDriver.setConnection();
    loadParams();
}

QList<ParamItem *>& ParamService::getPtrList(ParamItemType type) {
    if(type == ParamItemType::UPDATE) return ptrListUpdate;
    else if(type == ParamItemType::SET) return ptrListSet;
    else return ptrListUpdate;
}

bool ParamService::addParam(uchar incomeID, uchar incomeHostAddr, ParamItemType incomeType) {
    return addParam(ParamItem(incomeID, incomeHostAddr, incomeType));
}

bool ParamService::addParam(ParamItem &&paramItem) {
    auto mapKey = makeMapKey(paramItem);
    if(dataMap.contains(mapKey)) return false;
    dataMap.insert(mapKey, paramItem);
    auto* appendParam = (ParamItem *)&dataMap[mapKey];
    auto type = appendParam->getParamType();
    makeParamTimer(mapKey);
    if(type == ParamItemType::UPDATE) ptrListUpdate.append(appendParam);
    else if(type == ParamItemType::SET) ptrListSet.append(appendParam);
    writeParamToDB(mapKey);
    emit addedParamFromLine(type);
    return true;
}

bool ParamService::updateParam(const ProtosMessage &message, const QString &mapKey) {
    if(dataMap.contains(mapKey)){
        auto& paramToUpdate = dataMap[mapKey];
        paramToUpdate.update(message);
        auto paramType = paramToUpdate.getParamType();
        auto msgType = message.MsgType;
        if(timerMap.contains(mapKey)){
            if(paramType == SET && msgType == ProtosMessage::PANS)
                timerMap[mapKey]->stop();
            else
                timerMap[mapKey]->start(paramToUpdate.getUpdateRate());
        }
        writeParamToDB(mapKey);
        emit changedParamState(paramType);
        return true;
    }
    return false;
}

void ParamService::moveUpdateToSet(const QString &mapKey) {
    if(dataMap.contains(mapKey)){
        if(dataMap[mapKey].setParamType(SET)){
            makeParamTimer(mapKey);
            timerMap[mapKey]->stop();
            setPtrListFromMap(ParamItemType::UPDATE);
            setPtrListFromMap(ParamItemType::SET);
            emit needToResetModels();
        }
    }
}

void ParamService::makeParamTimer(const QString &mapKey) {
    if(!timerMap.contains(mapKey)) {
        auto* newTim = new QTimer(this);
        timerMap.insert(mapKey, newTim);
        timerMap[mapKey]->start(dataMap[mapKey].getUpdateRate());
        connect(newTim, &QTimer::timeout, [this, newTim]() {
            timerFinished(newTim->timerId());
        });
    }
}

QString ParamService::makeMapKey(uchar host, uchar ID){
    return QString("%1_%2").arg(ID,0,16).arg(host,0,16);
}

QString ParamService::makeMapKey(const ParamItem& paramItem){
    return QString("%1_%2").arg(paramItem.geParamId(), 0, 16).arg(paramItem.getHostID(), 0, 16);
}

bool ParamService::removeParam(const ParamItem &incomeParam) {
    auto mapKey = makeMapKey(incomeParam);
    if(!dataMap.contains(mapKey)) return false;
    auto removedType = dataMap[mapKey].getParamType();
    dataMap.remove(mapKey);
    if(timerMap.contains(mapKey)) timerMap.remove(mapKey);
    setPtrListFromMap(removedType);
    return true;
}

void ParamService::writeParamToDB(const QString &mapKey, const QString &event){
    auto& paramToWrite = dataMap[mapKey];
    if(!paramToWrite.isWriteToDb()) return;
    if(dbDriver.isDBOk()){
        if(!dbDriver.writeParam(paramToWrite, event)){
            paramToWrite.setState(DB_WRITE_FAIL);
            writeToFile = true;
            emit databaseWriteFail();
        }
    }else{
        paramToWrite.setState(DB_WRITE_FAIL);
        if(!writeToFile){
            writeToFile = true;
            emit databaseWriteFail();
        }
    }
    if(writeToFile)
        writeParamToFile(paramToWrite, event);
}

void ParamService::writeParamToFile(ParamItem &param, const QString &event) {
    if(!logFile->isOpen()){
        auto fileName = QDateTime::currentDateTime().toString(QString("yyyy.MM.dd_hh.mm.ss")).append(".csv");
        auto pathToLogs = QString(CURRENT_BUILD_TYPE_) == "Debug" ? "/../logs" : "/logs";
        logFile = new QFile(QCoreApplication::applicationDirPath() + QString("%1/%2").arg(pathToLogs).arg(fileName));
        logFile->open(QIODevice::ReadWrite);
        textStream.setDevice(logFile);
        textStream << param.getLogToFileHead();
    }
    textStream << param.getLogToFileStr(event);
}



void ParamService::configureTimer(int timerValue){
    writeTimer = new QTimer();
    connect(writeTimer, SIGNAL(timeout()), this, SLOT(writeTimerUpdate()));
    writeTimer->start(mSecWriteTimer);
}

void ParamService::writeTimerUpdate() {
//    qDebug("ParamService timer updated");
}

void ParamService::saveParams() {
    QJsonArray paramArr;
    for(auto& p: dataMap)
        paramArr.append(p.getJsonObject());
    qJsonObject["Params"] = paramArr;
    logFile->close();
}

void ParamService::loadParams() {
    QJsonArray paramArr = qJsonObject["Params"].toArray();
    dataMap.clear();
    timerMap.clear();
    for (const auto& param : paramArr)
    {
        QJsonObject loadParam = param.toObject();
        if (param != QJsonObject())
        {
            ParamItem item(loadParam);
            auto mapKey = makeMapKey(item);
            item.setState(OFFLINE);
            dataMap.insert(mapKey, item);
            makeParamTimer(mapKey);
        }
    }
    setPtrListFromMap(ParamItemType::UPDATE);
    setPtrListFromMap(ParamItemType::SET);
}

PSQL_Driver& ParamService::getDbDriver() {
    return dbDriver;
}

SocketAdapter& ParamService::getSocketAdapter() {
    return socketAdapter;
}

bool ParamService::isSocketConnected(){
    return socketAdapter.IsConnected();
}

void ParamService::setParamEvent(const QString &mapKey){
    auto& paramToWrite = dataMap[mapKey];
    auto senderId = paramToWrite.getSenderId() == selfAddr ? "localHost" : QString("%1").arg(paramToWrite.getSenderId(),0,16).toUpper().prepend("0x");
    auto eventStr = QString("SET_PARAM:%1 VALUE:%2 FROM:%3")
            .arg(paramToWrite.getTableName()).arg(paramToWrite.getValue().toString()).arg(senderId);
    logEventToDB(eventStr);
}

void ParamService::rxMsgHandler(const ProtosMessage &message) {
    QString mapKey;
    switch (message.MsgType){
        case ProtosMessage::MsgTypes::PANS:
            mapKey = makeMapKey(message.GetSenderAddr(), message.GetParamId());
            if(!updateParam(message, mapKey))
                addParam(std::move(ParamItem(message, UPDATE)));
            break;
        case ProtosMessage::MsgTypes::PSET:
            mapKey = makeMapKey(message.GetDestAddr(), message.GetParamId());
            if(!updateParam(message, mapKey))
                addParam(std::move(ParamItem(message, SET)));
            processSetParamReq(mapKey);
            break;
        default:
            break;
    }
}

void ParamService::txMsgHandler(const ProtosMessage &message) {

}

void ParamService::setPtrListFromMap(ParamItemType type){
    QList<ParamItem*>* list;
    if(type == ParamItemType::UPDATE) list = &ptrListUpdate;
    else if(type == ParamItemType::SET) list = &ptrListSet;
    else return;
    list->clear();
    auto it = dataMap.begin();
    auto end = dataMap.end();
    while(it != end){
        if(it.value().getParamType() == type)
            list->append(&it.value());
        it++;
    }
}

void ParamService::setParamValueChanged(int listPosition, const QVariant& value){
    auto* param = ptrListSet[listPosition];
    param->setExpectedValue(value);
    param->setSenderId(selfAddr);
    param->setState(PENDING);
    param->setLastValueType(ProtosMessage::MsgTypes::PSET);
    emit changedParamState(SET);
    auto mapKey = makeMapKey(*param);
    if(timerMap.contains(mapKey))
        timerMap[mapKey]->start(param->getUpdateRate());
    param->setValue(value);
    writeParamToDB(mapKey);
    sendProtosMsgSetParam(mapKey);
    processSetParamReq(mapKey);
}

void ParamService::processSetParamReq(const QString& mapKey){
    auto& param = dataMap[mapKey];
    ProtosMessage msg(param.getHostID(), selfAddr, param.geParamId(), ProtosMessage::MsgTypes::PREQ, ProtosMessage::ParamFields::VALUE);
    socketAdapter.SendMsg(msg);
}

void ParamService::sendProtosMsgSetParam(const QString &mapKey) {
    auto& param = dataMap[mapKey];
    ProtosMessage msg(param.getHostID(), selfAddr, param.geParamId(), short(param.getValue().toInt()), ProtosMessage::MsgTypes::PSET);
    socketAdapter.SendMsg(msg);
}

void ParamService::sortUpdateParamListAboutDB(bool arg){
    ptrListUpdate.clear();
    for(auto it=dataMap.begin(); it!=dataMap.end(); it++){
        if(it.value().getParamType() == UPDATE){
            if(!arg) ptrListUpdate.append(&it.value());
            else if(it.value().isWriteToDb())
                ptrListUpdate.append(&it.value());
        }
    }
}

void ParamService::sortUpdateListAboutOnline(bool arg) {
    ptrListUpdate.clear();
    for(auto it=dataMap.begin(); it!=dataMap.end(); it++){
        if(it.value().getParamType() == UPDATE){
            if(!arg) ptrListUpdate.append(&it.value());
            else if(it.value().getState() == ONLINE)
                ptrListUpdate.append(&it.value());
        }
    }
}

void ParamService::timerFinished(int timID) {
    for(auto it=timerMap.begin(); it!=timerMap.end(); it++){
        if(it.value()->timerId() == timID){
            it.value()->stop();
            if(dataMap.contains(it.key())) {
                dataMap[it.key()].timeoutUpdate();
                emit changedParamState(UPDATE);
            }
            break;
        }
    }
}

void ParamService::setSelfAddr(uchar incomeSelfAddr) {
    selfAddr = incomeSelfAddr;
}

uchar ParamService::getSelfAddr() const {
    return selfAddr;
}

void ParamService::setWriteToFile(bool incomeWriteToFile) {
    ParamService::writeToFile = incomeWriteToFile;
}

bool ParamService::isWriteToFile() const {
    return writeToFile;
}

void ParamService::closeAll() {
    dbDriver.closeConnection();
    logFile->close();
}

void ParamService::logEventToDB(const QString &eventStr) {
//    for(auto& param: dataMap.values())
//        if(makeMapKey(param) != mapKeySkip && param.getParamType() != SET) writeParamToDB(makeMapKey(param), eventStr);
    dbDriver.writeEvent(eventStr);
}

void ParamService::logViewChangesToDB(const QString& eventStr) {
    dbDriver.writeViewChanges(eventStr);
}