//
// Created by outlaw on 06.12.2022.
//

#include <QtWidgets/QLabel>
#include <ParamService.h>
#include <QtWidgets/QMessageBox>
#include "ConfigParamDlg.h"


ConfigParamDlg::ConfigParamDlg(ParamItem& paramItem, QWidget *parent):
        param(paramItem),
        offsetValueEdit(new QLineEdit(this)),
        multValueEdit(new QLineEdit(this)),
        updateRateValueEdit(new QLineEdit(this)),
        sendRateValueEdit(new QLineEdit(this)),
        updateRateValuesBtn(new QPushButton("Update Rate Values", this)),
        updateCalibValuesBtn(new QPushButton("Update Calib Values", this)),
        setRateValuesBtn(new QPushButton("Set Rate Values", this)),
        setCalibValuesBtn(new QPushButton("Set Calib Values", this)),
        offsetValueActual(new QLineEdit(this)),
        multValueActual(new QLineEdit(this)),
        updateRateValueActual(new QLineEdit(this)),
        sendRateValueActual(new QLineEdit(this))
{
    auto* mainLayout = new QHBoxLayout();
    mainLayout->setContentsMargins(10, 10, 10, 10);
    this->setLayout(mainLayout);
    auto label = new QLabel(QString("Param: %1 Host: %3").arg(param.getParamId(), 0, 16).arg(param.getHostID(), 0, 16));
    mainLayout->addWidget(label);
    mainLayout->addWidget(makeCalibBox());
    mainLayout->addWidget(makeRatesBox());
    show();
    raise();
}

QGroupBox* ConfigParamDlg::makeCalibBox(){
    auto* calibGroupBox = new QGroupBox(tr("Calibration Settings"), this);
    auto *calibGroupBoxLayout = new QFormLayout(calibGroupBox);
    calibGroupBox->setLayout(calibGroupBoxLayout);

    offsetValueActual->setAlignment(Qt::AlignCenter);
    offsetValueEdit->setAlignment(Qt::AlignCenter);
    offsetValueEdit->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::LineEdit));
    multValueActual->setAlignment(Qt::AlignCenter);
    multValueEdit->setAlignment(Qt::AlignCenter);
    multValueEdit->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::LineEdit));
    offsetValueActual->setEnabled(false);
    multValueActual->setEnabled(false);

    connect(updateCalibValuesBtn, &QPushButton::clicked, [this]()
    {
        offsetValueActual->clear();
        multValueActual->clear();
        offsetValueActual->setStyleSheet("background-color: bisque;font-weight: normal;");
        multValueActual->setStyleSheet("background-color: bisque;font-weight: normal;");
        makeMsg(ProtosMessage::OFFSET);
        makeMsg(ProtosMessage::MULT);
    });
    connect(setCalibValuesBtn, &QPushButton::clicked, [this]()
    {
        auto offsetText = offsetValueEdit->text();
        auto multText = multValueEdit->text();
        bool ok;
        if(offsetText.length()){
            float offsetValue = offsetText.toFloat(&ok);
            if(ok) makeMsg(ProtosMessage::OFFSET, offsetValue, true);
            else QMessageBox::information(this, "Wrong Offset Value", "Should be float");
        }
        if(multText.length()){
            float multValue = multText.toFloat(&ok);
            if(ok) makeMsg(ProtosMessage::MULT, multValue, true);
            else QMessageBox::information(this, "Wrong Mult value", "Should be float");
        }
    });
    calibGroupBoxLayout->addRow("Actual Offset value", offsetValueActual);
    calibGroupBoxLayout->addRow("Actual Mult value", multValueActual);
    calibGroupBoxLayout->addRow("Offset value", offsetValueEdit);
    calibGroupBoxLayout->addRow("Mult value", multValueEdit);
    calibGroupBoxLayout->addWidget(updateCalibValuesBtn);
    calibGroupBoxLayout->addWidget(setCalibValuesBtn);

    return calibGroupBox;
}

QGroupBox* ConfigParamDlg::makeRatesBox(){
    auto* ratesGroupBox = new QGroupBox(tr("Rates Settings"), this);
    auto* ratesGroupBoxLayout = new QFormLayout(ratesGroupBox);
    ratesGroupBox->setLayout(ratesGroupBoxLayout);

    updateRateValueActual->setAlignment(Qt::AlignCenter);
    updateRateValueEdit->setAlignment(Qt::AlignCenter);
    updateRateValueEdit->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::LineEdit));
    sendRateValueActual->setAlignment(Qt::AlignCenter);
    sendRateValueEdit->setAlignment(Qt::AlignCenter);
    sendRateValueEdit->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed, QSizePolicy::LineEdit));
    updateRateValueActual->setEnabled(false);
    sendRateValueActual->setEnabled(false);

    connect(updateRateValuesBtn, &QPushButton::clicked, [this]()
    {
        updateRateValueActual->clear();
        sendRateValueActual->clear();
        updateRateValueActual->setStyleSheet("background-color: bisque;font-weight: normal;");
        sendRateValueActual->setStyleSheet("background-color: bisque;font-weight: normal;");
        param.getParamType() == UPDATE ? makeMsg(ProtosMessage::UPDATE_RATE) : makeMsg(ProtosMessage::CTRL_RATE);
        makeMsg(ProtosMessage::SEND_RATE);
    });
    connect(setRateValuesBtn, &QPushButton::clicked, [this]()
    {
        auto updateRateText = updateRateValueEdit->text();
        auto sendRateText = sendRateValueEdit->text();
        bool ok;
        if(updateRateText.length()){
            short updateRateValue = updateRateText.toShort(&ok);
            if(ok) param.getParamType() == UPDATE ?
                makeMsg(ProtosMessage::UPDATE_RATE, updateRateValue, true)
                    : makeMsg(ProtosMessage::CTRL_RATE, updateRateValue, true);
            else QMessageBox::information(this, "Wrong Update Rate Value", "Should be short");
        }
        if(sendRateText.length()){
            short sendRateValue = sendRateText.toShort(&ok);
            if(ok) makeMsg(ProtosMessage::SEND_RATE, sendRateValue, true);
            else QMessageBox::information(this, "Wrong Send Rate value", "Should be short");
        }
    });

    ratesGroupBoxLayout->addRow("Actual Update/Control value", updateRateValueActual);
    ratesGroupBoxLayout->addRow("Actual SendRate value", sendRateValueActual);

    if(param.getParamType() == UPDATE)
        ratesGroupBoxLayout->addRow("Update Rate value", updateRateValueEdit);
    else if(param.getParamType() == SET)
        ratesGroupBoxLayout->addRow("Control Rate value", updateRateValueEdit);

    ratesGroupBoxLayout->addRow("SendRate value", sendRateValueEdit);
    ratesGroupBoxLayout->addWidget(updateRateValuesBtn);
    ratesGroupBoxLayout->addWidget(setRateValuesBtn);

    return ratesGroupBox;
}

template<typename T>
void ConfigParamDlg::makeMsg(ProtosMessage::ParamFields field, T value, bool set){
    ProtosMessage message = ProtosMessage(
            param.getHostID(),
            1,
            param.getParamId(),
            set ? ProtosMessage::PSET : ProtosMessage::PREQ,
            field,
            QVariant()
    );
    if(set) message.SetParamValue(value, true);
    emit newMsgToSend(message);
}

void ConfigParamDlg::processAns(const ProtosMessage& message) {
    auto value = QString("%1").arg(message.GetParamFieldValue().toFloat());
    switch (message.ParamField) {
        case ProtosMessage::CTRL_RATE:
        case ProtosMessage::UPDATE_RATE:
            updateRateValueActual->setText(value);
            updateRateValueActual->setStyleSheet("color: white; background-color: mediumaquamarine; font-weight: bold;");
            break;
        case ProtosMessage::SEND_RATE:
            sendRateValueActual->setText(value);
            sendRateValueActual->setStyleSheet("color: white; background-color: mediumaquamarine; font-weight: bold;");
            break;
        case ProtosMessage::MULT:
            offsetValueActual->setText(value);
            offsetValueActual->setStyleSheet("color: white; background-color: mediumaquamarine; font-weight: bold;");
            break;
        case ProtosMessage::OFFSET:
            multValueEdit->setText(value);
            multValueActual->setText(value);
            multValueActual->setStyleSheet("color: white; background-color: mediumaquamarine; font-weight: bold;");
            break;
        default:
            break;
    }
}