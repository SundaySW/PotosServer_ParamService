//
// Created by outlaw on 01.11.2022.
//
#include "Settings_dlg.h"


Settings_dlg::Settings_dlg(SocketAdapter* socketAdapter, PSQL_Driver& databaseDriver, QJsonObject& cfg, QWidget *parent)
    : QDialog(parent),
      qJsonObject(cfg)
{
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    setWindowTitle(QString("Settings"));
    auto* mainLayout = new QVBoxLayout(this);
    tabWidget = new QTabWidget;
    tabWidget->setTabShape(QTabWidget::Rounded);
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->setMovable(true);
    tabWidget->setTabBarAutoHide(true);
    auto connectWidget = new ServerConnectWidget(socketAdapter, qJsonObject);
    auto canWidget = new DBConfigWidget(qJsonObject, databaseDriver);
    tabWidget->addTab(connectWidget, "Server");
    tabWidget->addTab(canWidget, "DB");
    auto *gridLayout = new QGridLayout;
    gridLayout->addWidget(tabWidget);

    auto* btnLayout = new QHBoxLayout();
    btnLayout->setAlignment(Qt::AlignBottom);
    mainLayout->addLayout(gridLayout);
    mainLayout->addLayout(btnLayout);
    this->setLayout(mainLayout);
}
