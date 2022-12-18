//
// Created by outlaw on 01.11.2022.
//
#include "Settings_dlg.h"
#include "ParamServiceSettingsWidget.h"

Settings_dlg::Settings_dlg(ParamService* paramService, QJsonObject& cfg, QWidget *parent)
    : QDialog(parent)
{
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    setWindowTitle(QString("Settings"));
    auto* mainLayout = new QVBoxLayout(this);
    tabWidget = new QTabWidget;
    tabWidget->setTabShape(QTabWidget::Rounded);
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->setMovable(true);
    tabWidget->setTabBarAutoHide(true);
    auto pServerConnectWidget = new ServerConnectWidget(paramService->getSocketAdapter(), cfg);
    auto pDbConfigWidget = new DBConfigWidget(cfg, paramService->getDbDriver());
    auto pParamServiceSettingsWidget = new ParamServiceSettingsWidget(paramService, cfg);
    tabWidget->addTab(pServerConnectWidget, "Server");
    tabWidget->addTab(pDbConfigWidget, "DataBase");
    tabWidget->addTab(pParamServiceSettingsWidget, "ParamService");
    auto *gridLayout = new QGridLayout;
    gridLayout->addWidget(tabWidget);
    mainLayout->addLayout(gridLayout);
    this->setLayout(mainLayout);
    connect(pServerConnectWidget, &ServerConnectWidget::eventInServerConnection, [this](const QString& str, bool err){emit eventInSettings(str, err);});
    connect(pServerConnectWidget, &ServerConnectWidget::updateCountOfReConnections, [this](int n){emit updateNOfReConnections(n);});
    connect(pDbConfigWidget, &DBConfigWidget::eventInDBConnection, [this](){emit eventInSettings("", false);});
}
void Settings_dlg::closeEvent(QCloseEvent *event)
{
    close();
}