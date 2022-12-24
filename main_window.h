//
// Created by AlexKDR on 22.10.2022.
//
#ifndef POTOSSERVER_PARAMSERVICE_MAIN_WINDOW_H
#define POTOSSERVER_PARAMSERVICE_MAIN_WINDOW_H

#include <qmainwindow.h>
#include <QtWidgets/QTableView>
#include "IParamService_model.h"
#include <UpdateParamService_model.h>
#include <SetParamService_model.h>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QGroupBox>
#include <QtCore/QFile>
#include <view_ParamTabs/SetParamValueDlg.h>
#include "ParamItem.h"
#include "ParamService.h"
#include "Settings_dlg.h"
#include "UpdateParamsTab.h"
#include "SetParamsTab.h"


class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    MainWindow(int argv, char** argc, QWidget *parent = nullptr);
private slots:
    void OnClickedTableCell(const QModelIndex &index);
    void OnParamContextMenuReq(const QModelIndex&, IParamModel::ContextMenuReq);
protected:
    void timerEvent(QTimerEvent *event) override;
private:
    QLabel* serverLabel;
    QLabel* dbLabel;
    QWidget* CentralWin;
    QToolBar* Toolbar;
    QListWidget* logWidget;
    QTabWidget* tableTabWidget;
    UpdateParamsTab* updateParamsTab;
    SetParamsTab* setParamsTab;
    QCheckBox* logToFile;

    Settings_dlg* settingsDlg;
    QMap<QString, SetParamValueDlg*> setParamValueDlgMap;
    QSet<uchar> allHostsSet;
    QComboBox* hostSortCombBox;

    ParamService* paramService;
    QJsonObject ConfJson;
    QFile* configFile;

    void makeStatusBar();
    QToolBar* CreateToolbar();
    QGroupBox* makeParamSetGroupBox();
    QTabWidget *makeTabs();
    QListWidget* logWidgetConfig();
    void openFileLoadConfig();
    void AddToLog(const QString &string, bool isError = false);
    void closeEvent(QCloseEvent *event) override;
    void saveAll();
    void saveMainWindowSettings();
    void loadMainWindowSettings();
    void updateHostsSet();
    inline void updateServiceLabel(QLabel *label, bool status);
    void checkServicesConnection();
    QSplitter *makeSplitter();
    void saveLogToFile();
};
#endif //POTOSSERVER_PARAMSERVICE_MAIN_WINDOW_H