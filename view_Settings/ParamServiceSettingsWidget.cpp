//
// Created by 79162 on 18.12.2022.
//

#include <QtWidgets/QLabel>
#include "ParamServiceSettingsWidget.h"

ParamServiceSettingsWidget::ParamServiceSettingsWidget(ParamService *service, QJsonObject &JsonConf):
    paramService(service),
    savedConf(JsonConf),
    selfAddr(new QLineEdit(this)),
    requestOnSet(new QCheckBox(this)),
    saveBtn(new QPushButton("Save",this))
{
    Set();
    auto *layout = new QFormLayout();
    layout->setContentsMargins(30, 30, 30, 30);
    auto label = new QLabel("ParamService settings", this);
    label->setAlignment(Qt::AlignTop);
    label->setStyleSheet("font-weight: bold; padding: 5px; color: grey;");
    layout->addRow(label);
    layout->addRow(QString("Self Address"), selfAddr);
    layout->addRow(QString("Request on SET"),  requestOnSet);
    layout->addWidget(saveBtn);
    this->setLayout(layout);

    connect(requestOnSet, &QCheckBox::toggled, [this](bool checked)
    {
        paramService->setReqOnSet(checked);
    });

    selfAddr->setAlignment(Qt::AlignCenter);
    connect(selfAddr, &QLineEdit::textChanged, [this](const QString& newSelfAddr){
        paramService->setSelfAddr(newSelfAddr.toInt(nullptr,16));
    });
    connect(saveBtn, &QPushButton::clicked, [this]() {Save();});
}

void ParamServiceSettingsWidget::Set() {
    auto confObject = savedConf.value("ParamServiceConf");
    selfAddr->setText(QString("%1").arg(confObject["SelfAddr"].toString(),0,16));
    requestOnSet->setChecked(confObject["RequestOnSet"].toBool());
    paramService->setReqOnSet(confObject["RequestOnSet"].toBool());
}

void ParamServiceSettingsWidget::Save() {
    QJsonObject confObj;
    confObj["SelfAddr"] = selfAddr->text();
    confObj["RequestOnSet"] = requestOnSet->isChecked();
    savedConf["ParamServiceConf"] = confObj;
}
