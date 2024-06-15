#include "mainwindow.h"
# include <QDesktopWidget>
# include <QApplication>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)

{
    QDesktopWidget qd;
    setFixedSize(4*qd.width()/5,7*qd.height()/8);
    setWindowTitle("XOPTIMUS");
    mainWidget=new QWidget;
    setCentralWidget(mainWidget);
    mainWidget->setFixedSize(this->width(),this->height());
    mainLayout=new QVBoxLayout;
    mainWidget->setLayout(mainLayout);
    mainEdit=new QTextEdit;
    mainLayout->addWidget(mainEdit);
    mainEdit->setGeometry(2*this->width()/100,
                          2*this->height()/100,
                          97*this->width()/100,
                          95*this->height()/100);
    mainEdit->setReadOnly(true);
    QLabel *empty = new QLabel();
    empty->setText("  ");
    mainLayout->addWidget(empty);
    empty->setFixedHeight(3*this->height()/100);

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

void    MainWindow::addMessage(QString message)
{
    mainEdit->append(message);
}

void    MainWindow::unload()
{
    if(problemLoader!=NULL)
        delete problemLoader;
}
void    MainWindow::problemSlot(QAction *action)
{
    if(action->text()=="LOAD")
    {
        SelectProblemDialog *dialog = new SelectProblemDialog(
                    problemLoader->getSelectedProblemName(),
                    problemLoader->getProblemList(),
                    this
                    );
        dialog->setModal(true);
        dialog->exec();
        if(dialog->getSelectedName()!="")
        {
            myProblem=problemLoader->loadProblem(dialog->getSelectedName());
            addMessage("Load problem "+dialog->getSelectedName());
            //clear params
            QString s="{}";
            QJsonDocument doc = QJsonDocument::fromJson(s.toUtf8());
            QJsonObject obj=doc.object();
            problemLoader->setParams(obj);

        }
        else
            addMessage("None selected");
    }
    else
    if(action->text()=="TEST")
    {
        addMessage(""+problemLoader->getProblemReport());
    }
    else
    if(action->text()=="SEED")
    {
        bool ok;
            int i = QInputDialog::getInt(this, tr("Enter seed value"),
                                         tr("Random Seed:"), randomSeed, 0, INT_MAX, 1, &ok);
        if(ok)
        {
            randomSeed = i;
            addMessage("Set seed "+QString::number(randomSeed));
        }
        srand(randomSeed);
    }
    else
    if(action->text()=="PARAMS")
    {
        if(myProblem == NULL)
        {
            QMessageBox::warning(this, tr("XOPTIMUS"),
                                           tr("No problem loaded."),
                                           QMessageBox::Ok
                                          ,
                                           QMessageBox::Ok);
        }
        else
        {
            ParameterDialog *d = new ParameterDialog(
                        problemLoader->getParams(),
                        problemLoader->getSelectedProblemName(),this);
            d->setModal(true);
            int v=d->exec();

            if(v==QDialog::Accepted)
            {
                QJsonObject params = d->getParams();
                QJsonDocument doc(params);
                QString strJson(doc.toJson(QJsonDocument::Compact));
                addMessage("Problem params: "+strJson);
                problemLoader->setParams(params);
            }
        }
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

