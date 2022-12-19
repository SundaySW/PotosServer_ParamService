//
// Created by AlexKDR on 12.11.2022.
//
#include "IParamService_model.h"
#include "SetParamService_model.h"

int IParamService_model::rowCount(const QModelIndex &) const{
    return paramPtrList.size();
}

int IParamService_model::columnCount(const QModelIndex &parent) const {
    return ColCnt;
}

void IParamService_model::addRow() {
    beginInsertRows(QModelIndex(), paramPtrList.size(), paramPtrList.size());
    endInsertRows();
}
void IParamService_model::update(IParamModel::UPDATE_TASKS task) {
    switch (task) {
        case IParamModel::UPDATE_TASK:
            emit dataChanged(createIndex(0, 0), createIndex(0, ColCnt));
            break;
        case IParamModel::INSERT_ROW_TASK:
            addRow();
            break;
        case IParamModel::RESET_TASK:
            beginResetModel();
            endResetModel();
            break;
        default:
            break;
    }
}
void IParamService_model::removeRow(const QModelIndex &index){
    beginRemoveRows(index, index.row(), index.row());
    endRemoveRows();
}
bool IParamService_model::isDeleteCellClicked(const QModelIndex& index) {
    return index.column() == (ColCnt-1);
}

bool IParamService_model::isSetCellClicked(int col) {
    return col == IParamModel::VALUE;
}

const ParamItem* IParamService_model::getParam(int index){
//    if(index >= 0 and index < paramPtrList.size()) return paramPtrList[index];
//    if(index >= 0 and index < paramPtrList.size()) return paramPtrList.at(index);
    return paramPtrList.at(index);
//    else return nullptr;
}

bool IParamService_model::checkType(ParamItemType checkType) {
    return checkType == paramTypeOnModel;
}

ParamItemType IParamService_model::getType() const {
    return type;
}