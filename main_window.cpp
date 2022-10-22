//
// Created by AlexKDR on 22.10.2022.
//

#include "main_window.h"
#include "fixed_packet.h"
#include "packet_utils.h"
#include <algorithm>
#include <qapplication.h>
#include <qaction.h>
#include <qdockwidget.h>
#include <qjsondocument.h>
#include <qstackedwidget.h>
#include <qstatusbar.h>
#include <qboxlayout.h>
#include <qbuttongroup.h>
#include <qevent.h>
#include <qjsonarray.h>
#include <qicon.h>
#include <qmenu.h>
#include <qtcpsocket.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <QtWidgets/QDialogButtonBox>
#include <QtGui/QRegExpValidator>
#include "qfiledialog.h"

SocketAdapter Socket;

MainWindow::MainWindow(int argv, char** argc, QWidget *parent)
        : QMainWindow(parent)
        , CentralWin(new QWidget(parent))
        , Toolbar(nullptr)
        , Dlg(new ConnectionDialog(this, &Socket))
        , statusLabel(new QLabel("Status: ", this))
{
    Toolbar = CreateToolbar();
    auto* mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);
    statusLabel->setAlignment(Qt::AlignHCenter);
    statusLabel->setMinimumSize(statusLabel->sizeHint());
    statusBar()->addWidget(statusLabel);
    CentralWin->setLayout(mainLayout);
    setCentralWidget(CentralWin);
    Socket.AddTxMsgHandler([this](const ProtosMessage& txMsg) { txMsgHandler(txMsg);});

    if (!Socket.Connect("127.0.0.5", 3699, 1000)){
        statusLabel->setText(tr("Cant connect to Server"));
    }
    else{
        statusLabel->setText(tr("Connected to Server"));
    }
}

QToolBar* MainWindow::CreateToolbar()
{
    QToolBar* toolbar = addToolBar(QString());
    toolbar->setMovable(false);
    toolbar->setStyleSheet("QToolBar { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\ stop: 0 #f6f7fa, stop: 1 #dadbde); }");

    auto socketConnect = toolbar->addAction(QString("Connect Server"));
    socketConnect->setObjectName("ServerConnect");
    socketConnect->setCheckable(true);
    socketConnect->setToolTip(QStringLiteral("Подключиться/Отключиться к серверу"));
    connect(socketConnect, &QAction::triggered, [this](bool checked)
    {
        auto ConnDialog = new ConnectionDialog(this, &Socket);
        UpdatePortButton(ConnDialog->exec() != 0);
        delete ConnDialog;
    });
    toolbar->addSeparator();
    auto loadDevice = toolbar->addAction(QIcon(), QString("Add Device"));
    loadDevice->setObjectName("AddDevice");
    loadDevice->setCheckable(false);
    loadDevice->setToolTip(QStringLiteral("Добавить девайс"));
    connect(loadDevice, &QAction::triggered, [this](bool checked)
    {
        openFile();
    });

//    toolbar->addSeparator();
//    auto sendMSG = toolbar->addAction(QIcon(), QString("sendMSG"));
//    sendMSG->setObjectName("sendMSG");
//    sendMSG->setCheckable(false);
//    sendMSG->setToolTip(QStringLiteral("Отправить сообщ"));
//    connect(sendMSG, &QAction::triggered, [this](bool checked)
//    {
//        sendMessage();
//    });
//    toolbar->addSeparator();
//    auto getBlockOKMSG = toolbar->addAction(QIcon(), QString("getBlockOKMSG"));
//    getBlockOKMSG->setObjectName("getBlockOKMSG");
//    getBlockOKMSG->setCheckable(false);
//    getBlockOKMSG->setToolTip(QStringLiteral("Получить блок ОК"));
//    connect(getBlockOKMSG, &QAction::triggered, [this, &blockNum](bool checked)
//    {
//        getMessage();
//    });
//    toolbar->addSeparator();

    auto* expander = new QWidget(toolbar);
    expander->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    toolbar->addWidget(expander);

    auto exit = toolbar->addAction( QString("Exit"));
    exit->setToolTip(QStringLiteral("Закрыть приложение"));
    connect(exit, &QAction::triggered, [this](bool checked)
    {
        close();
    });
    return toolbar;
}

void MainWindow::UpdatePortButton(bool checked)
{
    auto btn = Toolbar->findChild<QAction*>("ServerConnect");
    btn->setText(checked ? "Connected" : "Disconnected");
//    btn->setIcon(QIcon(checked ? ":/plugin" : ":/plugout_red"));
    btn->setChecked(checked);
}

void MainWindow::getError(const QString &error, uint uid) {
    QDialog dlg(this);
    dlg.setWindowTitle(error);
    auto *layout = new QFormLayout();
    auto errorLabel = new QLabel(error, &dlg);
    auto questionLabel = new QLabel((QStringLiteral("Finish device FW with UID: %1").arg(uid,8,16)), &dlg);
    auto *btn_box = new QDialogButtonBox(&dlg);
    btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

    errorLabel->setAlignment(Qt::AlignHCenter);
    errorLabel->setMinimumSize(statusLabel->sizeHint());
    layout->addRow(errorLabel);
    layout->addRow(questionLabel);
    layout->addWidget(btn_box);

    dlg.setLayout(layout);
}

void MainWindow::openFile() {
    QDialog dlg(this);
    dlg.setWindowTitle(tr("AddDevice"));
    auto *uid = new QLineEdit("FFFFFF", &dlg);
    auto *addr = new QLineEdit("FF", &dlg);

    auto *btn_box = new QDialogButtonBox(&dlg);
    btn_box->setStandardButtons(QDialogButtonBox::Ok);
    connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);

    auto *layout = new QFormLayout();
    layout->addRow(tr("UID: "), uid);
    layout->addRow(tr("ADDR: "), addr);
    layout->addWidget(btn_box);

    dlg.setLayout(layout);
    if(dlg.exec() == QDialog::Accepted) {
        uint32_t uid24 = uid->text().toUInt(nullptr, 16);
        uint8_t addr8 = addr->text().toShort(nullptr, 16);
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Open Bin"), "/home", tr("Bin Files (*.bin)"));
//            QString fileName = "D:\\u\\sa_pico_can.bin";
        QFile file(fileName);
        if (file.open(QIODevice::ReadWrite))
        {
            qDebug() << (tr("UID: %1 ADDR: %2 ").arg(uid24).arg(addr8));
            statusLabel->setText(tr("Device loaded UID: %1 ADDR: %2 ").arg(uid24, 8, 16).arg(addr8, 2,16));
        }
    } else{
        statusLabel->setText(tr("Aborted adding device"));
    }
}

void MainWindow::txMsgHandler(const ProtosMessage &txMsg) {

}
