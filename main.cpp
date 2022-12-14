#include <iostream>
#include <qapplication.h>
#include <qmessagebox.h>
#include "main_window.h"

#ifdef _VER
    #define CURRENT_VER _VER
#else
    #define CURRENT_VER "CHECK CMAKE"
#endif

int main(int argv, char** argc)
{
    QApplication app(argv, argc);
    MainWindow mainWin(argv, argc);
    mainWin.setWindowTitle(QString("Protos Param Service %1").arg(CURRENT_VER));
    mainWin.resize(700, 800);
    mainWin.show();
    int res = QApplication::exec();
    return res;
}