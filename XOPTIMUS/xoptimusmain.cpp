#include "mainwindow.h"

#include <QApplication>

#include <QApplication>
#include <QSettings>
#include <QFont>
#include <QFontDialog>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setlocale(LC_ALL,"C");

 QSettings settings("MyCompany", "MyApp");

    // --- Αποθήκευση γραμματοσειράς ---
    if (!settings.contains("font")) {
        // Άνοιξε ένα font dialog να διαλέξεις
        bool ok;
        QFont font = QFontDialog::getFont(&ok, QFont("Verdana", 12));
        if (ok) {
            // Αποθήκευση σε string (Qt το κάνει portable)
            settings.setValue("font", font.toString());
        }
    }

    // --- Φόρτωση γραμματοσειράς ---
    QString fontString = settings.value("font").toString();
    QFont font;
    if (font.fromString(fontString)) {
        qDebug() << "Loaded font:" << font;
    } else {
        qDebug() << "Invalid font string:" << fontString;
    }

    MainWindow w;
    w.showMaximized();
    return a.exec();
}
