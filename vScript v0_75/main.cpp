#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

//border-width: 2px;
//border-color: #6d6969;
//border-style: dotted;
//background: #494c4c;
