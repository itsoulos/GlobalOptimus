#include "mainwindow.h"
# include <QDesktopWidget>
# include <QApplication>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)

{
    QDesktopWidget qd;
    setFixedSize(4*qd.width()/5,4*qd.height()/5);
    setWindowTitle("XOPTIMUS");
    mainWidget=new QWidget;
    setCentralWidget(mainWidget);
    mainWidget->setFixedSize(this->width(),this->height());
    mainLayout=new QVBoxLayout;
    mainWidget->setLayout(mainLayout);
    mainEdit=new QTextEdit;
    mainLayout->addWidget(mainEdit);
    mainEdit->setReadOnly(true);

    problemLoader  = new ProblemLoader();
    srand(randomSeed);

    //make menu
    problemMenu = new QMenu("PROBLEM");
    problemMenu->addAction("LOAD");
    problemMenu->addAction("TEST");
    problemMenu->addAction("PARAMS");
    problemMenu->addAction("SEED");
    problemMenu->addAction("QUIT");

    methodMenu  = new QMenu("METHOD");
    methodMenu->addAction("LOAD");
    methodMenu->addAction("PARAMS");
    methodMenu->addAction("STATISTICS");
    methodMenu->addAction("RUN");

    helpMenu    = new QMenu("HELP");
    helpMenu->addAction("MANUAL");
    helpMenu->addAction("ABOUT");

    menuBar()->addMenu(problemMenu);
    menuBar()->addMenu(methodMenu);
    menuBar()->addMenu(helpMenu);
    connect(problemMenu,SIGNAL(triggered(QAction*)),this,SLOT(problemSlot(QAction*)));
    connect(methodMenu,SIGNAL(triggered(QAction*)),this,SLOT(methodSlot(QAction*)));
    connect(helpMenu,SIGNAL(triggered(QAction*)),this,SLOT(helpSlot(QAction*)));
}

void    MainWindow::unload()
{
    if(problemLoader!=NULL)
        delete problemLoader;
}
void    MainWindow::problemSlot(QAction *action)
{
    if(action->text()=="SEED")
    {
        bool ok;
            int i = QInputDialog::getInt(this, tr("Enter seed value"),
                                         tr("Random Seed:"), randomSeed, 0, INT_MAX, 1, &ok);
        if(ok)
            randomSeed = i;
        srand(randomSeed);
    }
    else
    if(action->text()=="PARAMS")
    {
        ParameterDialog *d = new ParameterDialog("",this);
        d->show();
    }
    else
    if(action->text()=="QUIT")
    {
        unload();
        qApp->exit(0);
    }
}

void    MainWindow::methodSlot(QAction  *action)
{

}

void    MainWindow::helpSlot(QAction *action)
{

}
MainWindow::~MainWindow()
{
    unload();
}

