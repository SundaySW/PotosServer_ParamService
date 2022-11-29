//
// Created by outlaw on 24.10.2022.
//

#include <Monitor/protos_message.h>
#include <QtWidgets/QMessageBox>
#include "UpdateParamService_model.h"

const QString UpdateParamService_model::ColumnHeaders[UpdateParamService_model::nOfColumns] = {
        tr("DB"), tr("TimeStamp"), tr("ID"), tr("VALUE"), tr("NOTES")
};

UpdateParamService_model::UpdateParamService_model(QList<ParamItem*>& inList, QObject* parent):
    IParamService_model(inList,ParamItemType::UPDATE, nOfColumns, parent)
{
}

bool UpdateParamService_model::setData(const QModelIndex &index, const QVariant &value, int role)
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
        case DB_Active:
            if (role == Qt::CheckStateRole){
                data->setWriteToDb(value == Qt::Checked);
                update(UPDATE_TASK);
                return true;
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
    }
    return false;
}

QVariant UpdateParamService_model::data(const QModelIndex &index, int role) const {
    if (index.column() < 0 || index.column() > ColCnt || index.row() < 0 || index.row() > paramPtrList.size())
        return QVariant();
    auto& data = paramPtrList[index.row()];
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
            if(index.column() == DB_Active)
                return data->isWriteToDb() ? Qt::Checked : Qt::Unchecked;
            else return QVariant();
        default:
            return QVariant();
    }
}

QVariant UpdateParamService_model::GetDisplayRoleData(const QModelIndex& index, ParamItem *data) const
{
    switch (index.column())
    {
        case LastValueTime:
            return data->getLastValueTime() + " " + data->getLastValueDay();
        case PARAM_ID: {
            QString value = QString("0x%1").arg(data->geParamId(), 0, 16);
            value.append(QString("(from:0x%1)").arg(data->getHostID(),0, 16));
            if (data->getAltName().length())
                value = value.prepend(data->getAltName() + "(").append(")");
            return value;
        }
        case VALUE:
            return data->getValue().toString();
        case NOTES:
            return data->getNote();
        case DELETE:
            return QString("MOV/DEL");
    }
    return QVariant();
}

QBrush UpdateParamService_model::GetDisplayBackgroundRole(const QModelIndex& index, ParamItem* data) const
{
    switch (data->getState())
    {
        case ONLINE:
            if(data->isWriteToDb()) return QBrush(QColor("lightgreen"));
            else return QBrush(QColor("darkseagreen"));
        case OFFLINE:
            if(data->isWriteToDb()) return QBrush(QColor("lightgray"));
            else return QBrush(QColor("darkgray"));
        case DB_WRITE_FAIL:
            if(data->isWriteToDb()) return QBrush(QColor("tomato"));
        default:
            return QBrush(QColor("white"));
    }
}

Qt::ItemFlags UpdateParamService_model::flags(const QModelIndex& index) const {
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

QVariant UpdateParamService_model::headerData(int section, Qt::Orientation orientation, int role) const {
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
