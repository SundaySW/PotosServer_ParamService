//
// Created by outlaw on 28.10.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_PARAMSERVICE_H
#define POTOSSERVER_PARAMSERVICE_PARAMSERVICE_H

#include <PSQL_Driver.h>
#include <QtCore/QTimer>
#include <socket_adapter.h>
#include <IParamService_model.h>
#include <ConfigParamDlg.h>
#include "QJsonObject"
#include "QJsonArray"
#include "QJsonDocument"
#include "QFile"

class ParamService: public QObject{
    Q_OBJECT
public:
    explicit ParamService(QJsonObject &inJson);
    ParamService(const ParamService&) = delete;

    bool addParam(ParamItem &&paramItem, bool addToDB=true);
    bool addParam(uchar, uchar, ParamItemType);
    bool removeParam(const QString&);
    bool updateParam(const ProtosMessage &message, const QString &mapKey);
    void configureTimer(int);
    void saveParams();
    void loadParams();
    bool isSocketConnected();
    bool isDataBaseOk();
    PSQL_Driver& getDbDriver();
    QList<ParamItem *>& getPtrList(ParamItemType type);
    SocketAdapter& getSocketAdapter();
    void logEventToDB(const QString &eventStr);
    void setParamValueChanged(int, const QVariant& value);
    void logViewChangesToDB(const QString &);
    static QString makeMapKey(uchar host, uchar ID);
    static QString makeMapKey(const ParamItem &paramItem);
    void sortUpdateParamListAboutDB(bool);
    void sortUpdateListAboutOnline(bool);
    void sortAllParamsAboutHost(const QString &host);
    void sortByParamID(ParamItemType);
    void sortByHostID(ParamItemType);
    void setSelfAddr(uchar selfAddr);
    void setWriteToFile(bool writeToFile);
    bool isWriteToFile() const;
    void moveUpdateToSet(const QString &mapKey);
    uchar getSelfAddr() const;
    void closeAll();
    void configParam(const QString &mapKey);
    void setReqOnSet(bool _reqOnSet);
    bool isReqOnSet() const;
    QSet<uchar> getAllHostsInSet();
    void removeAllParams();
signals:
    void changedParamState(ParamItemType);
    void addedParamFromLine(ParamItemType);
    void needToResetModels();
    void databaseWriteFail();
    void errorInDBToLog(const QString&);
    void eventInDBToLog(const QString&);
private slots:
    void onNewConfigDlgMsg(ProtosMessage &message);
    void writeTimerUpdate();
private:
    uchar selfAddr;
    QMap<QString, ParamItem> dataMap;
    QList<ParamItem*> ptrListUpdate;
    QList<ParamItem*> ptrListSet;
    QMap<QString, QTimer*> timerMap;
    QMap<QString,ConfigParamDlg*> configParamDlgMap;

    QTimer* tim1;
    PSQL_Driver dbDriver;
    QTimer* writeTimer;
    int mSecWriteTimer = 1000;
    bool writeToFile;
    bool reqOnSet;
    QFile* logFile;
    QTextStream textStream;
    QJsonObject& qJsonObject;
    SocketAdapter socketAdapter;
    void setPtrListFromMap(ParamItemType type);
    void txMsgHandler(const ProtosMessage &message);
    void rxMsgHandler(const ProtosMessage &message);
    void timerFinished(int timID);
    void makeParamTimer(const QString &mapKey);
    void writeParamToDB(const QString &mapKey, const QString &event = "");
    void writeParamToFile(ParamItem &param, const QString &event);
    void setParamEvent(const QString &mapKey);
    void sendProtosMsgSetParam(const QString &mapKey);
    void processSetParamReq(const QString &mapKey);
    void manageTimersWhileUpdate(const QString &mapKey, uchar msgType, int updateRate, int paramType);
    void processPANSMsg(const ProtosMessage &message);
    void removeFromAllMaps(const QString &mapKey);
    void updateParamUpdateRate(const QString &mapKey, const QVariant &value);

    void processPSETMsg(const ProtosMessage &message);
};
#endif //POTOSSERVER_PARAMSERVICE_PARAMSERVICE_H