//
// Created by outlaw on 14.11.2022.
//

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include "SetParamsTab.h"
#include "QLineEdit"
#include "QDialog"

SetParamsTab::SetParamsTab(QList<ParamItem *>& list, QObject* parent)
        :setParamView(new QTableView(this)),
         setParamModel(new SetParamService_model(list, this)),
         addParam(new QPushButton("Add Param", this)),
         addParamDlg(new AddParamDlg(SET, this))
{
    auto *layout = new QVBoxLayout();
    setParamView->setModel(setParamModel);
    setParamView->setMinimumHeight(200);
    setParamView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    setParamView->setContextMenuPolicy(Qt::CustomContextMenu);
    setParamView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(setParamView);

    addParam->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed, QSizePolicy::CheckBox));

    auto* btnGroupBox = new QGroupBox();
    auto* btnGroupBoxLayout = new QHBoxLayout();
    btnGroupBoxLayout->addWidget(addParam);
    btnGroupBox->setLayout(btnGroupBoxLayout);
    btnGroupBoxLayout->setAlignment(Qt::AlignHCenter);
    layout->addWidget(btnGroupBox);

    connect(addParam, &QPushButton::clicked, [this](){
        addParamDlg->show();
        addParamDlg->raise();
        addParamDlg->activateWindow();
    });
    connect(addParamDlg, &AddParamDlg::haveParamToAdd, [this](uchar incomeID, uchar hostID, ParamItemType type){
        emit addedParamSig(incomeID, hostID, type);
    });
    connect(setParamView->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(UpdtData(const QModelIndex&, const QModelIndex&)));
    connect(setParamView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(TCClicked(const QModelIndex&)));
    this->setLayout(layout);
}

SetParamService_model* SetParamsTab::getModel() {
    return setParamModel;
}

void SetParamsTab::TCClicked(const QModelIndex& arg) {
    emit tableCellClicked(arg);
}

void SetParamsTab::UpdtData(const QModelIndex& arg, const QModelIndex& arg1) {
    emit tableCellDataUpdated(arg);
}