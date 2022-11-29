//
// Created by outlaw on 17.11.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_ADDPARAMDLG_H
#define POTOSSERVER_PARAMSERVICE_ADDPARAMDLG_H

#include <Param_Item/ParamItem.h>
#include "QDialog"

class AddParamDlg  : public QDialog{
    Q_OBJECT
public:
    explicit AddParamDlg(ParamItemType, QWidget *parent = nullptr);
signals:
    void haveParamToAdd(uchar incomeID, uchar hostID, ParamItemType);
private:
    uchar ID;
    uchar HostAddr;
    ParamItemType paramType;
    void closeEvent(QCloseEvent *event) override;
};


#endif //POTOSSERVER_PARAMSERVICE_ADDPARAMDLG_H
