//
// Created by outlaw on 22.11.2022.
//

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include "QPushButton"
#include "SetParamValueDlg.h"
#include "QVariant"

SetParamValueDlg::SetParamValueDlg(uchar pId, uchar hAddr, QWidget *parent):
        QDialog(parent),
        paramID(pId),
        HostAddr(hAddr)
{
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    setWindowTitle(QString("Set Protos Param 0x%1 Value on host: 0x%2").arg(paramID,0,16).arg(HostAddr,0,16));
    auto *layout = new QFormLayout();
    auto label = new QLabel(QString("Set Value for 0x%1 param on 0x%2 host").arg(paramID,0,16).arg(HostAddr,0,16), this);
    auto valueEdit = new QLineEdit("");
    auto* setBtn = new QPushButton("Set");
    label->setAlignment(Qt::AlignHCenter);
    layout->addRow(label);
    layout->addRow(tr(""), valueEdit);
    layout->addWidget(setBtn);
    setLayout(layout);
    connect(setBtn, &QPushButton::clicked, [this, valueEdit]()
    {
        bool isConverted = false;
        ushort setValue = valueEdit->text().toInt(&isConverted);
        if(!isConverted || setValue <= 0 || setValue > 4095){
            QMessageBox::information(this, "Wrong value", "should be (short) > 0 && <= 4095");
            return;
        }
        auto setVal = QVariant(setValue);
        emit paramValueSet(paramID, HostAddr, setVal);
    });
}

void SetParamValueDlg::closeEvent(QCloseEvent *event) {
}