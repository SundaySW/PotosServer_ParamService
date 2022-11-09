//
// Created by outlaw on 02.11.2022.
//

#include "DBConfigWidget.h"

DBConfigWidget::DBConfigWidget(QJsonObject &JsonConf, PSQL_Driver& databaseDriver)
        : HostName(new QLineEdit(this))
        , DatabaseName(new QLineEdit(this))
        , UserName(new QLineEdit(this))
        , Password(new QLineEdit(this))
        , Port(new QLineEdit(this))
        , autoconnect(new QCheckBox(this))
        , connectBtn(new QPushButton(tr("Save+Connect"), this))
        , statusLabel(new QLabel("", this))
        , savedConf(JsonConf)
        , dbDriver(databaseDriver)
{
    Set();
    auto *layout = new QFormLayout();
    layout->setContentsMargins(30, 30, 30, 30);
    auto label = new QLabel("DataBase connection", this);
    label->setAlignment(Qt::AlignTop);
    layout->addRow(label);
    layout->addRow(QString("Host Name"), HostName);
    layout->addRow(QString("Database Name"),  DatabaseName);
    Password->setEchoMode(QLineEdit::Password);
    layout->addRow(QString("User Name"), UserName);
    layout->addRow(QString("Password"),  Password);
    layout->addRow(QString("Port"), Port);
    layout->addRow(QString("Auto connect"), autoconnect);
    layout->addWidget(connectBtn);
    connect(connectBtn, &QPushButton::clicked, [this](){
        dbDriver.setConnection();
    });
    this->setLayout(layout);
}

void DBConfigWidget::Save() {
    QJsonObject confObj;
    confObj["HostName"] = HostName->text();
    confObj["DatabaseName"] = DatabaseName->text();
    confObj["UserName"] = UserName->text();
    confObj["Password"] = Password->text();
    confObj["Port"] = Port->text();
    confObj["autoconnect"] = autoconnect->isChecked();
    savedConf["DBConfObject"] = confObj;
}

void DBConfigWidget::Set() {
    auto confObject = savedConf.value("DBConfObject");
    HostName->setText((confObject["HostName"].toString()));
    DatabaseName->setText (confObject["DatabaseName"].toString());
    UserName->setText(confObject["UserName"].toString());
    Password->setText  (confObject["Password"].toString());
    Port->setText(confObject["Port"].toString());
    autoconnect->setChecked (confObject["autoconnect"].toBool());
}