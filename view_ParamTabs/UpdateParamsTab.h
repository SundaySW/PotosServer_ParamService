//
// Created by AlexKDR on 13.11.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_UPDATEPARAMSTAB_H
#define POTOSSERVER_PARAMSERVICE_UPDATEPARAMSTAB_H

#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <UpdateParamService_model.h>
#include "QWidget"
#include "QCheckBox"
#include "QTableView"
#include "AddParamDlg.h"

class UpdateParamsTab : public QWidget{
    Q_OBJECT
public:
    UpdateParamsTab(QList<ParamItem *> &, QObject* = nullptr);
    UpdateParamService_model* getModel();
signals:
    void addedParamSig(uchar, uchar, ParamItemType);
    void tableCellClicked(const QModelIndex&);
    void tableCellDataUpdated(const QModelIndex&);
    void onlyDBParamShow(bool);
    void onlyOnlineParamShow(bool);
private slots:
    void TCClicked(const QModelIndex&);
    void UpdtData(const QModelIndex&, const QModelIndex&);
private:
    QTableView* updateView;
    UpdateParamService_model* updateParamModel;
    QPushButton* addParam;
    QCheckBox* hideOffline;
    QCheckBox* onlyInDB;
    AddParamDlg* addParamDlg;
};


#endif //POTOSSERVER_PARAMSERVICE_UPDATEPARAMSTAB_H
