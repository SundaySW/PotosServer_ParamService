//
// Created by outlaw on 01.11.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_ISETTINGSWIDGET_H
#define POTOSSERVER_PARAMSERVICE_ISETTINGSWIDGET_H

#include <QtWidgets/QWidget>
#include <Monitor/socket_adapter.h>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include "QFormLayout"
#include <QtWidgets/QLabel>
#include "QJsonObject"
#include "QCheckBox"

class ISettingsWidget
{
public:
    virtual void Save(QJsonObject& conf) = 0;
    virtual void Set(QJsonObject &conf) = 0;
};


#endif //POTOSSERVER_PARAMSERVICE_ISETTINGSWIDGET_H