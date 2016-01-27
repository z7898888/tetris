#include "mainwindow.h"
#include <QApplication>
#include <networkwidget.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //NetworkWidget nw;
    //nw.show();
    return a.exec();
}
