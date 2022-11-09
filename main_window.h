//
// Created by AlexKDR on 22.10.2022.
//
#ifndef POTOSSERVER_PARAMSERVICE_MAIN_WINDOW_H
#define POTOSSERVER_PARAMSERVICE_MAIN_WINDOW_H

#include <qmainwindow.h>
#include <QtWidgets/QTableView>
#include <TableModel/ParamService_model.h>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QGroupBox>
#include <QtCore/QFile>
#include "connection_dialog.h"
#include "ParamItem.h"
#include "ParamService.h"
#include "Settings/Settings_dlg.h"

class QTcpSocket;

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    MainWindow(int argv, char** argc, QWidget *parent = nullptr);
    //signals:
private slots:
    void addParam();
    void OnClickedTableCell(const QModelIndex &index);
protected:
    void timerEvent(QTimerEvent *event) override;
private:
    void openFileLoadConfig();
    void txMsgHandler(const ProtosMessage &txMsg);

    QTableView* makeParamTable();
    QToolBar* CreateToolbar();
    void makeStatusLabel();
    QGroupBox *makeParamSetGroupBox();
    void closeEvent(QCloseEvent *event);
    void saveAll();
    QListWidget* logWidgetConfig();
    void AddToLog(const QString &string, bool isError = false);

    QLabel* serverLabel;
    QLabel* dbLabel;
    QWidget* CentralWin;
    QToolBar* Toolbar;
    QTableView *TableView;
    ParamService_model* paramServiceModel;
    Settings_dlg* settingsDlg;
    ParamService* paramService;
    SocketAdapter Socket;
    int updateTimerID;
    bool logToFileFlag;
    QJsonObject ConfJson;
    QFile* configFile;
    QListWidget* logWidget;

    void checkServicesConnection();
};
#endif //POTOSSERVER_PARAMSERVICE_MAIN_WINDOW_H