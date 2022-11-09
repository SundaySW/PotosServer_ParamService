//
// Created by outlaw on 27.10.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_PARAMITEM_H
#define POTOSSERVER_PARAMSERVICE_PARAMITEM_H


#include <QtCore/QtGlobal>
#include <QtCore/QVariant>
#include "QDateTime"
#include "QJsonObject"

class ParamItem {

public:
    enum States
    {
        InDB = 0,
        No_DB = 1,
        ONLINE = 2,
        OFFLINE = 3,
        ERROR = 4,
    };

    ParamItem() = default;
    explicit ParamItem(uchar);
    explicit ParamItem(QJsonObject&);
    ParamItem(uchar, QString, bool);

    [[nodiscard]] uchar getId() const;
    [[nodiscard]] const QVariant &getValue() const;
    [[nodiscard]] const QString &getNote() const;
    [[nodiscard]] States getState() const;
    [[nodiscard]] bool isWriteToDb() const;

    void setValue(const QVariant &value);
    void setNote(const QString &note);
    void setState(States inState);
    void setWriteToDb(bool writeToDb);
    void setAltName(const QString&);
    void setLastValueTime(const QDateTime& lastValueTime);
    const QString& getAltName();
    QString getLastValueTime();
    QString getLastValueDay();
    QJsonObject getJsonObject();
private:
    QDateTime lastValueTime;
    uchar ID;
    QString altName;
    QVariant Value;
    QString Note;
    States state;
    bool writeToDB;
};


#endif //POTOSSERVER_PARAMSERVICE_PARAMITEM_H
