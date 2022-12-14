//
// Created by outlaw on 24.10.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_SetParamService_model_MODEL_H
#define POTOSSERVER_PARAMSERVICE_SetParamService_model_MODEL_H

#include <QtCore/QAbstractTableModel>
#include <Param_Item/ParamItem.h>
#include "Monitor/protos_message.h"
#include <QBrush>
#include "IParamService_model.h"

class SetParamService_model : public IParamService_model
{
public:

    explicit SetParamService_model(QList<ParamItem*>&, QObject* parent = nullptr);
    ~SetParamService_model() override = default;

    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex&, const QVariant&, int) Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex&) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const  Q_DECL_OVERRIDE;
private:
    QVariant GetDisplayRoleData(const QModelIndex&, ParamItem*) const;
    QBrush GetDisplayBackgroundRole(const QModelIndex&, ParamItem*) const;
    static const int nOfColumns = 5;
    static const QString ColumnHeaders[nOfColumns];
};

#endif //POTOSSERVER_PARAMSERVICE_UPDATEPARAMSERVICE_MODEL_H
