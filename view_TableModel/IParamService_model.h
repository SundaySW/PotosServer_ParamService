//
// Created by AlexKDR on 12.11.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_IPARAMSERVICE_MODEL_H
#define POTOSSERVER_PARAMSERVICE_IPARAMSERVICE_MODEL_H

#include "QAbstractTableModel"
#include "ParamItem.h"

class IParamService_model: public QAbstractTableModel
{
    Q_OBJECT
public:
    enum UPDATE_TASKS{
        UPDATE_TASK = 0,
        INSERT_ROW_TASK = 1,
        DELETE_ROW_TASK = 2,
        RESET_TASK = 3
    };

    IParamService_model(QList<ParamItem*>& inList, ParamItemType paramType, int colCount, QObject* parent):
        QAbstractTableModel(parent),
        paramPtrList(inList),
        paramTypeOnModel(paramType),
        ColCnt(colCount),
        type(paramType)
    {}

    int rowCount(const QModelIndex&) const Q_DECL_OVERRIDE;
    void addRow();
    void removeRow(const QModelIndex &index);
    void update(UPDATE_TASKS task);
    bool isDeleteCellClicked(const QModelIndex& index);
    const ParamItem* getParam(const QModelIndex &index);
    int columnCount(const QModelIndex& parent) const Q_DECL_OVERRIDE;
    bool checkType(ParamItemType);
    bool isSetCellClicked(int);

protected:
    QList<ParamItem*>& paramPtrList;
    ParamItemType type;
public:
    ParamItemType getType() const;

protected:
    const int ColCnt;
    const ParamItemType paramTypeOnModel;
};
#endif //POTOSSERVER_PARAMSERVICE_IPARAMSERVICE_MODEL_H
