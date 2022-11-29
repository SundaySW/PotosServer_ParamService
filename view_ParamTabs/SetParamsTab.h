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
    SetParamsTab(QList<ParamItem *>&, QObject* = nullptr);
    SetParamService_model* getModel();
signals:
    void addedParamSig(uchar, uchar, ParamItemType);
    void tableCellClicked(const QModelIndex&);
    void tableCellDataUpdated(const QModelIndex&);
private slots:
    void TCClicked(const QModelIndex&);
    void UpdtData(const QModelIndex&, const QModelIndex&);
private:
    QTableView* setParamView;
    SetParamService_model* setParamModel;
    QPushButton* addParam;
    AddParamDlg* addParamDlg;
};

#endif //POTOSSERVER_PARAMSERVICE_SETPARAMSTAB_H
