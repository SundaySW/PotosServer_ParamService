//
// Created by outlaw on 01.11.2022.
//
#include "Settings_dlg.h"

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
    tabWidget->addTab(pServerConnectWidget, "Server");
    tabWidget->addTab(pDbConfigWidget, "DB");
    auto *gridLayout = new QGridLayout;
    gridLayout->addWidget(tabWidget);
    mainLayout->addLayout(gridLayout);
    this->setLayout(mainLayout);
    connect(pServerConnectWidget, &ServerConnectWidget::eventInServerConnection, [this](const QString& str, bool err){emit eventInSettings(str, err);});
    connect(pDbConfigWidget, &DBConfigWidget::eventInServerConnection, [this](){emit eventInSettings("", false);});
}
void Settings_dlg::closeEvent(QCloseEvent *event)
{
    emit settingsDialogClosed();
    close();
}