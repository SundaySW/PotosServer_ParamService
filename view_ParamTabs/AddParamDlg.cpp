//
// Created by outlaw on 17.11.2022.
//

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include "AddParamDlg.h"
#include "QPushButton"

AddParamDlg::AddParamDlg(ParamItemType type, QWidget *parent):
    QDialog(parent),
    paramType(type)
{
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    setWindowTitle(QString("Add Protos Param"));
    auto *layout = new QFormLayout();
    auto label = new QLabel("Add Param", this);
    auto IDEdit = new QLineEdit("");
    auto HostIDEdit = new QLineEdit("");
    auto* addBtn = new QPushButton("Add");
    label->setAlignment(Qt::AlignHCenter);
    layout->addRow(label);
    layout->addRow(tr("ID (in hex): "), IDEdit);
    layout->addRow(tr("Host Addr (in hex): "), HostIDEdit);
    layout->addWidget(addBtn);
    setLayout(layout);
    connect(addBtn, &QPushButton::clicked, [this, IDEdit, HostIDEdit]()
    {
        bool ok = false;
        ID = IDEdit->text().toInt(&ok,16);
        if(!ok || ID <= 0 || ID > 255){
            QMessageBox::information(this, "Wrong ID", "ID should be (uint8) > 0 && <= 255");
            return;
        }
        HostAddr = HostIDEdit->text().toInt(&ok,16);
        if(!ok || HostAddr <= 0 || HostAddr > 255){
            QMessageBox::information(this, "Wrong HostID", "HostID should be (uint8) > 0 && <= 255");
            return;
        }
        emit haveParamToAdd(ID, HostAddr, paramType);
    });
}

void AddParamDlg::closeEvent(QCloseEvent *event) {
}