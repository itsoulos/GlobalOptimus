#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
# include <QVBoxLayout>
# include <QHBoxLayout>
# include <QLabel>
# include <QMenu>
# include <QMenuBar>
# include <QTextEdit>
# include <QInputDialog>
# include <QMessageBox>
# include <XOPTIMUS/parameterdialog.h>
# include <XOPTIMUS/problemloader.h>
# include <XOPTIMUS/selectproblemdialog.h>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QWidget *mainWidget;
    QVBoxLayout *mainLayout;
    QTextEdit *mainEdit;
    QMenuBar *menubar;
    QMenu *problemMenu,*methodMenu, *helpMenu;
    ProblemLoader *problemLoader;
    Problem       *myProblem;
    int randomSeed =1;
    void unload();
public:
    MainWindow(QWidget *parent = nullptr);
    void    addMessage(QString message);
    ~MainWindow();
public slots:
    void problemSlot(QAction *action);
    void methodSlot(QAction  *action);
    void helpSlot(QAction *action);
};
#endif // MAINWINDOW_H
