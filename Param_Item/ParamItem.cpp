//
// Created by outlaw on 27.10.2022.
//

#include "ParamItem.h"

#include <utility>

 ParamItem::ParamItem(uchar incomeID)
    :ID(incomeID),
    Value (QObject::tr("")),
    Note(("")),
    writeToDB(false),
    state(No_DB)
{
}

ParamItem::ParamItem(QJsonObject& obj)
{
    ID = obj["ID"].toInt();
    altName = obj["altName"].toString();
    Value = obj["Value"].toString();;
    Note = obj["Note"].toString();
    writeToDB = obj["writeToDB"].toBool();
    state = States(obj["state"].toInt());
    lastValueTime = QDateTime::fromString(obj["dateTime"].toString());
}


ParamItem::ParamItem(uchar inID, QString inNote, bool inToDB)
    :ID(inID),
    Note(std::move(inNote)),
    writeToDB(inToDB)
{
    state = inToDB?InDB:No_DB;
}

uchar ParamItem::getId() const {
    return ID;
}

const QVariant &ParamItem::getValue() const {
    return Value;
}

const QString &ParamItem::getNote() const {
    return Note;
}

ParamItem::States ParamItem::getState() const {
    return state;
}

bool ParamItem::isWriteToDb() const {
    return writeToDB;
}

void ParamItem::setValue(const QVariant &value) {
    Value = value;
}

void ParamItem::setNote(const QString &note) {
    Note = note;
}

void ParamItem::setState(ParamItem::States inState) {
    ParamItem::state = inState;
}

void ParamItem::setWriteToDb(bool writeToDb) {
    writeToDB = writeToDb;
}

QString ParamItem::getLastValueTime(){
    if(lastValueTime.isNull())
        return QString("-- --");
    return lastValueTime.toString(QString("hh:mm:ss:zzz"));
}

QString ParamItem::getLastValueDay() {
    if(lastValueTime.isNull())
        return QString("-- --");
    return lastValueTime.toString(QString("dd.mm"));
}

void ParamItem::setLastValueTime(const QDateTime& valueTime) {
    ParamItem::lastValueTime = valueTime;
}

void ParamItem::setAltName(const QString& name) {
    altName = name;
}

const QString& ParamItem::getAltName() {
    return altName;
}

QJsonObject ParamItem::getJsonObject() {
    QJsonObject retVal;
    retVal["ID"] = ID;
    retVal["altName"] = altName;
    retVal["Value"] = Value.toString();
    retVal["Note"] = Note;
    retVal["state"] = static_cast<int>(state);
    retVal["writeToDB"] = writeToDB;
    retVal["dateTime"] = lastValueTime.toString();
    return retVal;
}