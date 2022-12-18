//
// Created by 79162 on 18.12.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_PARAMSERVICESETTINGSWIDGET_H
#define POTOSSERVER_PARAMSERVICE_PARAMSERVICESETTINGSWIDGET_H


#include <QtWidgets/QWidget>
#include <ParamService.h>
#include <QtWidgets/QCheckBox>

class ParamServiceSettingsWidget  : public QWidget
{
    Q_OBJECT
public:
    ParamServiceSettingsWidget(ParamService* service, QJsonObject& JsonConf);
private:
    ParamService* paramService;
    QLineEdit* selfAddr;
    QCheckBox* requestOnSet;
    QPushButton* saveBtn;
    QJsonObject& savedConf;

    void Set();

    void Save();
};


#endif //POTOSSERVER_PARAMSERVICE_PARAMSERVICESETTINGSWIDGET_H
