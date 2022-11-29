//
// Created by AlexKDR on 13.11.2022.
//

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include "UpdateParamsTab.h"
#include "QDialog"
#include "QLineEdit"

UpdateParamsTab::UpdateParamsTab(QList<ParamItem *>& list, QObject* parent)
    :updateView(new QTableView(this)),
    updateParamModel(new UpdateParamService_model(list, this)),
    addParam(new QPushButton("Add Param", this)),
    hideOffline(new QCheckBox("Hide offline",this)),
    onlyInDB(new QCheckBox("Only in DB", this)),
    addParamDlg(new AddParamDlg(UPDATE, this))
{
    auto *layout = new QVBoxLayout();
    updateView->setModel(updateParamModel);
    updateView->setMinimumHeight(200);
    updateView->setColumnWidth(0,20);
    updateView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    updateView->setContextMenuPolicy(Qt::CustomContextMenu);
    updateView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(updateView);

    addParam->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed,QSizePolicy::ToolButton));
    hideOffline->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed,QSizePolicy::CheckBox));
    onlyInDB->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed,QSizePolicy::CheckBox));

    auto* btnGroupBox = new QGroupBox();
    auto* btnGroupBoxLayout = new QHBoxLayout();
    btnGroupBoxLayout->addWidget(addParam);
    btnGroupBoxLayout->addWidget(hideOffline);
    btnGroupBoxLayout->addWidget(onlyInDB);
    btnGroupBoxLayout->setAlignment(Qt::AlignHCenter);
    btnGroupBox->setLayout(btnGroupBoxLayout);
    layout->addWidget(btnGroupBox);
    this->setLayout(layout);

    connect(addParam, &QPushButton::clicked, [this]() {
        addParamDlg->show();
        addParamDlg->raise();
        addParamDlg->activateWindow();
    });
    connect(addParamDlg, &AddParamDlg::haveParamToAdd, [this](uchar incomeID, uchar hostID, ParamItemType type){
        emit addedParamSig(incomeID, hostID, type);
    });
    connect(onlyInDB, &QCheckBox::toggled, [this](bool checked) {
        emit onlyDBParamShow(checked);
    });
    connect(hideOffline, &QCheckBox::toggled, [this](bool checked) {
        emit onlyOnlineParamShow(checked);
    });

    connect(updateView->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(UpdtData(const QModelIndex&, const QModelIndex&)));
    connect(updateView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(TCClicked(const QModelIndex&)));
}

UpdateParamService_model* UpdateParamsTab::getModel() {
    return updateParamModel;
}

void UpdateParamsTab::TCClicked(const QModelIndex& arg){
    emit tableCellClicked(arg);
}

void UpdateParamsTab::UpdtData(const QModelIndex &arg, const QModelIndex &arg1) {
    emit tableCellDataUpdated(arg);
}
