//
// Created by outlaw on 14.11.2022.
//

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include <ParamService.h>
#include <UpdateParamService_model.h>
#include "SetParamsTab.h"
#include "QLineEdit"
#include "QDialog"
#include "QMenu"
#include "QHeaderView"


SetParamsTab::SetParamsTab(ParamService* Service, QObject* parent):
        paramService(Service),
        setParamView(new QTableView(this)),
        setParamModel(new SetParamService_model(paramService->getPtrList(CONTROL), this)),
        addParam(new QPushButton("Add Param", this)),
        addParamDlg(new AddParamDlg(CONTROL, this))
{
    auto *layout = new QVBoxLayout();
    setParamView->setModel(setParamModel);
    setParamView->setMinimumHeight(200);
    setParamView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    setParamView->setContextMenuPolicy(Qt::CustomContextMenu);
    setParamView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setParamView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setParamView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);

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
        paramService->addParam(incomeID, hostID, type);
    });
    connect(setParamView->model(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(UpdtData(const QModelIndex&, const QModelIndex&)));
    connect(setParamView->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(on_sectionClicked(int)));
    connect(setParamView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(TCClicked(const QModelIndex&)));
    connect(setParamView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ContextMenuRequested(const QPoint&)));

    this->setLayout(layout);
}

SetParamService_model* SetParamsTab::getModel() {
    return setParamModel;
}

void SetParamsTab::TCClicked(const QModelIndex& arg) {
    emit tableCellClicked(arg);
}

void SetParamsTab::UpdtData(const QModelIndex& index, const QModelIndex& arg1) {
//    if(index.column() == SetParamService_model::Headers::VALUE)
//    paramService->setParamValueChanged(index.row());
    auto* param = setParamModel->getParam(index.row());
    if(param == nullptr) return;
    auto tableName = param->getTableName();
    if(index.column() == IParamModel::PARAM_ID || index.column() == IParamModel::PARAM_ID){
        paramService->logViewChangesToDB(QString("%1 : new alt_name : %2").arg(tableName).arg(param->getAltName()));
    }
    else if(index.column() == IParamModel::NOTES || index.column() == IParamModel::NOTES){
        paramService->logViewChangesToDB(QString("%1 : new note : %2").arg(tableName).arg(param->getNote()));
    }
}

void SetParamsTab::ContextMenuRequested(const QPoint& pos) {
//    auto curr_tableview = qobject_cast<QTableView *>(sender());
    QModelIndex index = setParamView->indexAt(pos);
    if(index.isValid())
    {
        auto* menu = new QMenu(this);
        auto* configParam = new QAction(QString("Config"), this);
        auto* deleteParam = new QAction(QString("Delete"), this);
        connect(configParam, &QAction::triggered, this, [this, index](){
            emit ParamContextMenuReq(index, IParamModel::ContextMenuReq::Config);
        });
        connect(deleteParam, &QAction::triggered, this, [this, index](){
            emit ParamContextMenuReq(index, IParamModel::ContextMenuReq::Delete);
        });
        menu->addAction(configParam);
        menu->addAction(deleteParam);
        menu->popup(setParamView->viewport()->mapToGlobal(pos));
    }
}
void SetParamsTab::on_sectionClicked(int col) {
    if(col == IParamModel::PARAM_ID) paramService->sortByParamID(getModel()->getType());
    else if(col == IParamModel::PARAM_HOST) paramService->sortByHostID(getModel()->getType());
    getModel()->update(IParamModel::RESET_TASK);
}