//
// Created by outlaw on 06.12.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_CONFIGPARAMDLG_H
#define POTOSSERVER_PARAMSERVICE_CONFIGPARAMDLG_H

#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLineEdit>
#include <Monitor/protos_message.h>
#include <Param_Item/ParamItem.h>
#include "QPushButton"
#include "QDialog"

class ConfigParamDlg : public QDialog{
    Q_OBJECT
public:
    enum ConfigParamType{
        Rates,
        Calib
    };
    explicit ConfigParamDlg(ParamItem&, QWidget *parent = nullptr);
    void processAns(const ProtosMessage&);
signals:
    void newMsgToSend(ProtosMessage&);
private:
    ParamItem& param;
    QGroupBox* makeCalibBox();
    QGroupBox* makeRatesBox();
    QLineEdit* updateRateValueEdit;
    QLineEdit* sendRateValueEdit;
    QLineEdit* offsetValueEdit;
    QLineEdit* multValueEdit;
    QLineEdit* updateRateValueActual;
    QLineEdit* sendRateValueActual;
    QLineEdit* offsetValueActual;
    QLineEdit* multValueActual;
    QPushButton* updateCalibValuesBtn;
    QPushButton* updateRateValuesBtn;
    QPushButton* setCalibValuesBtn;
    QPushButton* setRateValuesBtn;
    template<typename T = short >
    void makeMsg(ProtosMessage::ParamFields field, T value = 0, bool set = false);
};


#endif //POTOSSERVER_PARAMSERVICE_CONFIGPARAMDLG_H
