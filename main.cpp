#include <iostream>
#include <qapplication.h>
#include <qmessagebox.h>
#include "main_window.h"

int main(int argv, char** argc)
{
    QApplication app(argv, argc);
    MainWindow mainWin(argv, argc);
//    mainWin.setWindowTitle(QStringLiteral("CAN firmware loader"));
    mainWin.resize(700, 550);
    mainWin.show();
    int res = app.exec();
    return res;
}