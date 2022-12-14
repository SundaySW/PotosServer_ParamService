//
// Created by outlaw on 14.11.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_SETPARAMSTAB_H
#define POTOSSERVER_PARAMSERVICE_SETPARAMSTAB_H

#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <SetParamService_model.h>
#include "QWidget"
#include "QCheckBox"
#include "QTableView"
#include "AddParamDlg.h"

class SetParamsTab : public QWidget{
    Q_OBJECT
public:
    SetParamsTab(ParamService* Service, QObject* = nullptr);
    SetParamService_model* getModel();
signals:
    void addedParamSig(uchar, uchar, ParamItemType);
    void tableCellClicked(const QModelIndex&);
    void tableCellDataUpdated(const QModelIndex&);
    void ParamContextMenuReq(const QModelIndex&, IParamModel::ContextMenuReq);
    void sortByParamID(ParamItemType, IParamModel::Headers);

private slots:
    void TCClicked(const QModelIndex&);
    void UpdtData(const QModelIndex&, const QModelIndex&);
    void ContextMenuRequested(const QPoint&);
    void on_sectionClicked(int);

private:
    ParamService* paramService;
    QTableView* setParamView;
    SetParamService_model* setParamModel;
    QPushButton* addParam;
    AddParamDlg* addParamDlg;
};

#endif //POTOSSERVER_PARAMSERVICE_SETPARAMSTAB_H
