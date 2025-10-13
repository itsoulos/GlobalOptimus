#include "mainwindow.h"

#include <QApplication>

#include <QApplication>
#include <QSettings>
#include <QFont>
#include <QFontDialog>
#include <QDebug>
# include <QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setlocale(LC_ALL,"C");
    // Φόρτωση του QSS αρχείου
    QFile file(":/XOPTIMUS/light.css"); // μπορεί να είναι και από το filesystem
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        a.setStyleSheet(styleSheet);
    }
    QFont font;
    font.setPointSize(14); // Μέγεθος γραμματοσειράς
    a.setFont(font);
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
