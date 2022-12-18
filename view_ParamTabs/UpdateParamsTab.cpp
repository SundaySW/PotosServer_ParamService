//
// Created by AlexKDR on 13.11.2022.
//

#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <ParamService.h>
#include <QtCore/QSortFilterProxyModel>
#include "UpdateParamsTab.h"
#include "QDialog"
#include "QLineEdit"
#include "QMenu"
#include "QHeaderView"

UpdateParamsTab::UpdateParamsTab(ParamService* Service, QObject* parent):
    paramService(Service),
    updateView(new QTableView(this)),
    updateParamModel(new UpdateParamService_model(paramService->getPtrList(UPDATE), this)),
    addParam(new QPushButton("Add Param", this)),
    hideOffline(new QCheckBox("Hide offline",this)),
    onlyInDB(new QCheckBox("Only in DB", this)),
    addParamDlg(new AddParamDlg(UPDATE, this)),
    removeAllParams(new QPushButton("Remove All", this))
{
    auto *layout = new QVBoxLayout();
    updateView->setModel(updateParamModel);
    updateView->setMinimumHeight(200);
    updateView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    updateView->setContextMenuPolicy(Qt::CustomContextMenu);
    updateView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    updateView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    updateView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

    layout->addWidget(updateView);

    addParam->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed,QSizePolicy::ToolButton));
    removeAllParams->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed,QSizePolicy::ToolButton));
    hideOffline->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed,QSizePolicy::CheckBox));
    onlyInDB->setSizePolicy(QSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed,QSizePolicy::CheckBox));

    auto* btnGroupBox = new QGroupBox();
    auto* btnGroupBoxLayout = new QHBoxLayout();
    btnGroupBoxLayout->addWidget(addParam);
    btnGroupBoxLayout->addWidget(hideOffline);
    btnGroupBoxLayout->addWidget(onlyInDB);
    btnGroupBoxLayout->addWidget(removeAllParams);
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
        paramService->addParam(incomeID, hostID, type);
    });
    connect(onlyInDB, &QCheckBox::toggled, [this](bool checked) {
        paramService->sortUpdateParamListAboutDB(checked);
        getModel()->update(IParamModel::RESET_TASK);
    });
    connect(hideOffline, &QCheckBox::toggled, [this](bool checked) {
        paramService->sortUpdateListAboutOnline(checked);
        getModel()->update(IParamModel::RESET_TASK);
    });
    connect(removeAllParams, &QPushButton::clicked, [this](){
        paramService->removeAllParams();
    });

    connect(updateView->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(UpdtData(const QModelIndex&, const QModelIndex&)));
    connect(updateView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(TCClicked(const QModelIndex&)));
    connect(updateView->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(on_sectionClicked(int)));
    connect(updateView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(TCClicked(const QModelIndex&)));
    connect(updateView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ContextMenuRequested(const QPoint&)));
}

UpdateParamService_model* UpdateParamsTab::getModel() {
    return updateParamModel;
}

void UpdateParamsTab::TCClicked(const QModelIndex& arg){
    emit tableCellClicked(arg);
}

void UpdateParamsTab::UpdtData(const QModelIndex &index, const QModelIndex &arg1) {
    auto* param = updateParamModel->getParam(index.row());
    auto tableName = param->getTableName();
    if(index.column() == IParamModel::PARAM_ID || index.column() == IParamModel::PARAM_ID){
        paramService->logViewChangesToDB(QString("%1 : new alt_name : %2").arg(tableName).arg(param->getAltName()));
    }
    else if(index.column() == IParamModel::NOTES || index.column() == IParamModel::NOTES){
        paramService->logViewChangesToDB(QString("%1 : new note : %2").arg(tableName).arg(param->getNote()));
    }
}

void UpdateParamsTab::ContextMenuRequested(const QPoint& pos) {
//    auto curr_tableview = qobject_cast<QTableView *>(sender());
    QModelIndex index = updateView->indexAt(pos);
    if(index.isValid())
    {
        auto* menu = new QMenu(this);
        auto* configParam = new QAction(QString("Config"), this);
        auto* deleteParam = new QAction(QString("Delete"), this);
//        auto* moveParam = new QAction(QString("Move"), this);
        connect(configParam, &QAction::triggered, this, [this, index](){
            emit ParamContextMenuReq(index, IParamModel::ContextMenuReq::Config);
        });
        connect(deleteParam, &QAction::triggered, this, [this, index](){
            emit ParamContextMenuReq(index, IParamModel::ContextMenuReq::Delete);
        });
//        connect(moveParam, &QAction::triggered, this, [this, index](){
//            emit ParamContextMenuReq(index, IParamModel::ContextMenuReq::Move);
//        });
        menu->addAction(configParam);
        menu->addAction(deleteParam);
//        menu->addAction(moveParam);
        menu->popup(updateView->viewport()->mapToGlobal(pos));
    }
}

void UpdateParamsTab::on_sectionClicked(int col) {
    if(col == IParamModel::PARAM_ID) paramService->sortByParamID(UPDATE);
    else if(col == IParamModel::PARAM_HOST) paramService->sortByHostID(UPDATE);
    getModel()->update(IParamModel::RESET_TASK);
}
