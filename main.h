//
// Created by AlexKDR on 22.10.2022.
//

#ifndef POTOSSERVER_PARAMSERVICE_MAIN_H
#define POTOSSERVER_PARAMSERVICE_MAIN_H

#include <qmainwindow.h>
#include "connection_dialog.h"

class QTcpSocket;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(int argv, char** argc, QWidget *parent = 0);
    //signals:
private slots:

private:
    QToolBar* CreateToolbar();
    QLabel* statusLabel;
    QWidget* CentralWin;
    QToolBar* Toolbar;
    std::unique_ptr<ConnectionDialog> Dlg;
    void txMsgHandler(const ProtosMessage &txMsg);
};
#endif //POTOSSERVER_PARAMSERVICE_MAIN_H
