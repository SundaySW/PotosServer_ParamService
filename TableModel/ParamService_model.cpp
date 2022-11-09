//
// Created by outlaw on 24.10.2022.
//

#include <Monitor/protos_message.h>
#include <QtWidgets/QMessageBox>
#include "ParamService_model.h"

const QString ParamService_model::ColumnHeaders[ParamService_model::ColCnt] = {
        tr("DB"), tr("Day"), tr("Time"), tr("ID"), tr("VALUE"), tr("NOTES")
};

ParamService_model::ParamService_model(QList<ParamItem*>& inList, QObject* parent):
    QAbstractTableModel(parent)
    ,paramPtrList(inList)
{
}

int ParamService_model::columnCount(const QModelIndex &parent) const {
    return ColCnt;
}

bool ParamService_model::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    auto* data = paramPtrList[index.row()];
    switch (index.column()) {
        case PARAM_ID:
            if(role == Qt::EditRole){
                if(value.canConvert(QVariant::String) && value.toString().size()){
                    data->setAltName(value.toString());
                    return true;
                }
            }
            break;
        case DB_Active:
            if (role == Qt::CheckStateRole){
                data->setWriteToDb(value == Qt::Checked);
                data->setState(value == Qt::Checked ? ParamItem::States::InDB : ParamItem::States::No_DB);
                update();
                return true;
            }
            break;
        case NOTES:
            if(role == Qt::EditRole){
                if(value.canConvert(QVariant::String) && value.toString().size()){
                  data->setNote(value.toString());
                  return true;
                }
            }
            break;
    }
    return false;
}

QVariant ParamService_model::data(const QModelIndex &index, int role) const {
    if (index.column() < 0 || index.column() > ColCnt || index.row() < 0 || index.row() > paramPtrList.size())
        return QVariant();
    auto& data = paramPtrList[index.row()];
    switch (role)
    {
        case Qt::BackgroundRole:
            return GetDisplayBackgroundRole(index, data);
        case Qt::DisplayRole:
            return GetDisplayRoleData(index, data);
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::CheckStateRole:
            if(index.column() == DB_Active)
                return data->isWriteToDb() ? Qt::Checked : Qt::Unchecked;
            else return QVariant();
        default:
            return QVariant();
    }
}

QVariant ParamService_model::GetDisplayRoleData(const QModelIndex& index, ParamItem *data) const
{
    switch (index.column())
    {
        case Time_Day:
            return data->getLastValueDay();
        case Time_Time:
            return data->getLastValueTime();
        case PARAM_ID: {
            QString value = QString("%1").arg(data->getId(), 4, 16, QChar('0')).toUpper().prepend("0x");
            if (data->getAltName().length())
                value = value.prepend(data->getAltName() + "(").append(")");
            return value;
        }
        case VALUE:
            return data->getValue().toString();
        case NOTES:
            return data->getNote();
        case DELETE:
            return QString("DELETE ME");
    }
    return QVariant();
}

QBrush ParamService_model::GetDisplayBackgroundRole(const QModelIndex& index, ParamItem* data) const
{
    switch (data->getState())
    {
        case ParamItem::InDB:
            return QBrush(QColor("lightgreen"));
        default:
            return QBrush(QColor("white"));
    }
}

Qt::ItemFlags ParamService_model::flags(const QModelIndex& index) const {
    switch (index.column()) {
        case DB_Active:
            return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
            break;
        case NOTES:
        case PARAM_ID:
            return Qt::ItemIsEditable | Qt::ItemIsEnabled;
            break;
        default:
            break;
    }
    return Qt::NoItemFlags;
}

QVariant ParamService_model::headerData(int section, Qt::Orientation orientation, int role) const {
    switch (role)
    {
        case Qt::DisplayRole:
            if(orientation == Qt::Horizontal)
                return section < ColCnt ? ColumnHeaders[section] : QVariant();
            else
                return section + 1;
        default:
            return QVariant();
    }
}

int ParamService_model::rowCount(const QModelIndex &) const{
    return paramPtrList.size();
}

void ParamService_model::addRow() {
    beginInsertRows(QModelIndex(), paramPtrList.size(), paramPtrList.size());
    endInsertRows();
}

void ParamService_model::removeRow(const QModelIndex &index) {
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    endRemoveRows();
}

void ParamService_model::update() {
    emit dataChanged(createIndex(0, 0), createIndex(0, ColCnt));
}

bool ParamService_model::isDeleteCellClicked(const QModelIndex& index) {
    return index.column() == DELETE;
}

int ParamService_model::getParamID(const QModelIndex& index){
    return paramPtrList[index.row()]->getId();
}