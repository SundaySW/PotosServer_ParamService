//
// Created by outlaw on 24.10.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_PARAMSERVICE_MODEL_H
#define POTOSSERVER_PARAMSERVICE_PARAMSERVICE_MODEL_H

#include <QtCore/QAbstractTableModel>
#include <Param_Item/ParamItem.h>
#include "Monitor/protos_message.h"
#include <QBrush>

class ParamService_model : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Headers
    {
        DB_Active,
        Time_Day,
        Time_Time,
        PARAM_ID,
        VALUE,
        NOTES,
        DELETE
    };
    explicit ParamService_model(QList<ParamItem*>&, QObject* parent = nullptr);
    ~ParamService_model() override = default;

    int columnCount(const QModelIndex& parent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex&, const QVariant&, int) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex&) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const  Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex&) const Q_DECL_OVERRIDE;
    void addRow();
    void removeRow(const QModelIndex &index);
    void update();
    bool isDeleteCellClicked(const QModelIndex& index);
    int getParamID(const QModelIndex &index);

private:
    QVariant GetDisplayRoleData(const QModelIndex&, ParamItem*) const;
    QBrush GetDisplayBackgroundRole(const QModelIndex&, ParamItem*) const;
    QList<ParamItem*>& paramPtrList;
    static const int ColCnt = 7;
    static const QString ColumnHeaders[ColCnt];
};

#endif //POTOSSERVER_PARAMSERVICE_PARAMSERVICE_MODEL_H
