//
// Created by outlaw on 24.10.2022.
//

#include <Monitor/protos_message.h>
#include <QtWidgets/QMessageBox>
#include "SetParamService_model.h"

const QString SetParamService_model::ColumnHeaders[SetParamService_model::nOfColumns] = {
        tr("TimeStamp"), tr("ID"), tr("VALUE"), tr("NOTES")
};

SetParamService_model::SetParamService_model(QList<ParamItem*>& inList, QObject* parent):
    IParamService_model(inList,ParamItemType::SET, nOfColumns, parent)
{
}

bool SetParamService_model::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;
    auto* data = paramPtrList[index.row()];
    if(data->getParamType() != paramTypeOnModel)
        return false;
    switch (index.column()) {
        case PARAM_ID:
            if(role == Qt::EditRole){
                if(value.canConvert(QVariant::String) && value.toString().size()){
                    data->setAltName(value.toString());
                    emit dataChanged(index, index);
                    return true;
                }
            }
            break;
        case NOTES:
            if(role == Qt::EditRole){
                if(value.canConvert(QVariant::String) && value.toString().size()){
                  data->setNote(value.toString());
                    emit dataChanged(index, index);
                  return true;
                }
            }
            break;
//        case VALUE:
//            if(role == Qt::EditRole){
//                bool isConverted = false;
//                ushort setValue = value.toInt(&isConverted);
//                if(isConverted){
//                    data->setValue(setValue);
//                    emit dataChanged(index, index);
//                    return true;
//                } else return false;
//            }
//            break;
        default:
            break;
    }
    return false;
}

QVariant SetParamService_model::data(const QModelIndex &index, int role) const{
    if (index.column() < 0 || index.column() > ColCnt || index.row() < 0 || index.row() > paramPtrList.size())
        return QVariant();
    auto* data = paramPtrList[index.row()];
    if(data->getParamType() != paramTypeOnModel)
        return QVariant();
    switch (role)
    {
        case Qt::BackgroundRole:
            return GetDisplayBackgroundRole(index, data);
        case Qt::DisplayRole:
            return GetDisplayRoleData(index, data);
        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;
        case Qt::CheckStateRole:
        default:
            return QVariant();
    }
}

QVariant SetParamService_model::GetDisplayRoleData(const QModelIndex& index, ParamItem *data) const
{
    switch (index.column())
    {
        case LastValueTime:
            return data->getLastValueTime() + " " + data->getLastValueDay();
        case PARAM_ID: {
            QString value = QString("0x%1").arg(data->geParamId(), 0, 16);
            value.append(QString("(from:0x%1)").arg(data->getHostID(),0, 16));
            if (data->getAltName() == " ") return value;
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

QBrush SetParamService_model::GetDisplayBackgroundRole(const QModelIndex& index, ParamItem* data) const
{
    switch (data->getState())
    {
        case ONLINE:
            return QBrush(QColor("lightgreen"));
        case OFFLINE:
            return QBrush(QColor("lightcoral"));
        case PENDING:
            return QBrush(QColor("bisque"));
        case ERROR:
        case DB_WRITE_FAIL:
            return QBrush(QColor("tomato"));
        default:
            return QBrush(QColor("white"));
    }
}

Qt::ItemFlags SetParamService_model::flags(const QModelIndex& index) const {
    switch (index.column()) {
        case NOTES:
//        case VALUE:
        case PARAM_ID:
            return Qt::ItemIsEditable | Qt::ItemIsEnabled;
            break;
        default:
            break;
    }
    return Qt::NoItemFlags;
}

QVariant SetParamService_model::headerData(int section, Qt::Orientation orientation, int role) const {
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

