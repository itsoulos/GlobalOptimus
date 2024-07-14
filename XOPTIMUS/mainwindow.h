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
    QMenu *loadMenu, *settingsMenu, *executeMenu, *helpMenu;
    ProblemLoader *problemLoader;
    MethodLoader  *methodLoader;
    Problem       *myProblem;
    Optimizer     *myMethod;
    QString       methodName;
    int randomSeed =1;
    int ntimes =1;
    const QString appName = "Xoptimus";
    void unload();
    void noProblemLoaded();
    void noMethodLoaded();
    void    startRunning();
    void    endRunning();
    Statistics *myStat;
    QTabWidget *mainTab;
    QTextEdit *lastEdit;
    QString lastProblem="";
    QString lastMethod ="";
    QRect qd;
    void    showMessageBox(QString title,QString message);
public:
    MainWindow(QWidget *parent = nullptr);
    void    addMessage(QString message,bool running=false);
    ~MainWindow();
public slots:
    void loadSlot(QAction *action);
    void settingsSlot(QAction  *action);
    void executeSlot(QAction  *action);
    void helpSlot(QAction *action);
protected:
     void closeEvent(QCloseEvent *event);
};
#endif // MAINWINDOW_H
