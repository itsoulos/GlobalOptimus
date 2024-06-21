#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <iostream>
#include <stdio.h>
#include <unistd.h>
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
# include <QFileSystemWatcher>
# include <QTabWidget>
# include <XOPTIMUS/parameterdialog.h>
# include <XOPTIMUS/problemloader.h>
# include <XOPTIMUS/selectproblemdialog.h>
# include <XOPTIMUS/methodloader.h>
# include <XOPTIMUS/selectmethoddialog.h>
# include <OPTIMUS/statistics.h>
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
    MethodLoader  *methodLoader;
    Problem       *myProblem;
    Optimizer     *myMethod;
    QString       methodName;
    int randomSeed =1;
    int ntimes =1;
    void unload();
    void noProblemLoaded();
    void noMethodLoaded();
    void    startRunning();
    void    endRunning();
    int std_fd;
    QFileSystemWatcher * watcher;
    Statistics *myStat;
    QTabWidget *mainTab;
    QTextEdit *lastEdit;
    QString lastProblem="";
    QString lastMethod ="";
public:
    MainWindow(QWidget *parent = nullptr);
    void    addMessage(QString message,bool running=false);
    ~MainWindow();
public slots:
    void problemSlot(QAction *action);
    void methodSlot(QAction  *action);
    void helpSlot(QAction *action);
    void fileChanged(QString text);
protected:
     void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
