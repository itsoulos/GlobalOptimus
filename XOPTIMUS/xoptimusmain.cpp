#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setlocale(LC_ALL,"C");
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
