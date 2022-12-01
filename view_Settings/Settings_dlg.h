//
// Created by outlaw on 01.11.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_SETTINGS_DLG_H
#define POTOSSERVER_PARAMSERVICE_SETTINGS_DLG_H

#include <QtWidgets/QDialog>
#include "QComboBox"
#include "QHBoxLayout"
#include "ServerConnectWidget.h"
#include "DBConfigWidget.h"
#include <QtWidgets/QPushButton>
#include "QButtonGroup"
#include <QGroupBox>
#include <socket_adapter.h>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QSplitter>
#include <DB_Driver/PSQL_Driver.h>
#include <ParamService.h>
#include "QJsonObject"

class Settings_dlg : public QDialog
{
    Q_OBJECT
public:
    Settings_dlg(ParamService* paramService, QJsonObject&, QWidget *parent = nullptr);
signals:
    void eventInSettings(const QString&, bool);
private:
    QTabWidget* tabWidget;
    void closeEvent(QCloseEvent *event) override;
};


#endif //POTOSSERVER_PARAMSERVICE_SETTINGS_DLG_H
