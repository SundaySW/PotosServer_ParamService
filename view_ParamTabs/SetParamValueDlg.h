//
// Created by outlaw on 22.11.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_SETPARAMVALUE_H
#define POTOSSERVER_PARAMSERVICE_SETPARAMVALUE_H

#include "QDialog"

class SetParamValueDlg : public QDialog{
    Q_OBJECT
public:
    explicit SetParamValueDlg(uchar, uchar, QWidget *parent = nullptr);
signals:
    void paramValueSet(uchar incomeID, uchar hostID, const QVariant& value);
private:
    uchar paramID;
    uchar HostAddr;
    void closeEvent(QCloseEvent *event) override;
};


#endif //POTOSSERVER_PARAMSERVICE_SETPARAMVALUE_H
