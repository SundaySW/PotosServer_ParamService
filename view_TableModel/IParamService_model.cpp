//
// Created by AlexKDR on 12.11.2022.
//
#include "IParamService_model.h"
#include "SetParamService_model.h"

int IParamService_model::rowCount(const QModelIndex &) const{
    int retVal = 0;
    for(auto p:paramPtrList)
        if(p->getParamType() == paramTypeOnModel) retVal++;
    return retVal;
}

int IParamService_model::columnCount(const QModelIndex &parent) const {
    return ColCnt;
}

void IParamService_model::addRow() {
    beginInsertRows(QModelIndex(), paramPtrList.size(), paramPtrList.size());
    endInsertRows();
}
void IParamService_model::update(UPDATE_TASKS task) {
    switch (task) {
        case UPDATE_TASK:
            emit dataChanged(createIndex(0, 0), createIndex(0, ColCnt));
            break;
        case INSERT_ROW_TASK:
            addRow();
            break;
        case RESET_TASK:
            beginResetModel();
            endResetModel();
            break;
        default:
            break;
    }
}
void IParamService_model::removeRow(const QModelIndex &index){
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    endRemoveRows();
}
bool IParamService_model::isDeleteCellClicked(const QModelIndex& index) {
    return index.column() == (ColCnt-1);
}

bool IParamService_model::isSetCellClicked(int col) {
    return col == SetParamService_model::Headers::VALUE;
}

const ParamItem* IParamService_model::getParam(const QModelIndex& index){
    return paramPtrList[index.row()];
}

bool IParamService_model::checkType(ParamItemType checkType) {
    return checkType == type;
}

ParamItemType IParamService_model::getType() const {
    return type;
}
