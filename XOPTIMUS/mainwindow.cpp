#include "mainwindow.h"
# include <QApplication>
# include <QScreen>
# include <OPTIMUS/editlogger.h>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)

{

	QScreen *screen = QGuiApplication::primaryScreen();
     qd = screen->geometry();

    this->setGeometry((qd.width()/2)-(this->width()/2),
                       (qd.height()/2)-(this->height()/2),
                       7*qd.width()/8,7*qd.height()/8);

    setWindowTitle("XOPTIMUS");
    mainWidget=new QWidget;
    setCentralWidget(mainWidget);
    mainLayout=new QVBoxLayout;
    mainWidget->setLayout(mainLayout);
    mainTab=new QTabWidget;

    mainEdit=new QTextEdit;
    mainTab->addTab(mainEdit,"Main");
    mainLayout->addWidget(mainTab);

    mainEdit->setReadOnly(true);
    QLabel *empty = new QLabel();
    empty->setText("  ");
    mainLayout->addWidget(empty);

    problemLoader  = new ProblemLoader();
    myMethod = NULL;
    methodLoader   = new MethodLoader();
    srand(randomSeed);
    myStat = NULL;
    myProblem = NULL;
    //make menu
    loadMenu = new QMenu("LOAD");
    loadMenu->addAction("PROBLEM");
    loadMenu->addAction("METHOD");
    loadMenu->addAction("QUIT");

    settingsMenu  = new QMenu("SETTINGS");
    settingsMenu->addAction("PROBLEM");
    settingsMenu->addAction("METHOD");
    settingsMenu->addAction("SEED");

    executeMenu = new QMenu("EXECUTE");
    executeMenu->addAction("RUN");
    executeMenu->addAction("STATISTICS");
    executeMenu->addAction("TEST");


    helpMenu    = new QMenu("HELP");
    helpMenu->addAction("TEAM");
    helpMenu->addAction("MANUAL");
    helpMenu->addAction("ABOUT");
    lastEdit = NULL;
    menuBar()->addMenu(loadMenu);
    menuBar()->addMenu(settingsMenu);
    menuBar()->addMenu(executeMenu);
    menuBar()->addMenu(helpMenu);
    connect(loadMenu,SIGNAL(triggered(QAction*)),this,SLOT(loadSlot(QAction*)));
    connect(settingsMenu,SIGNAL(triggered(QAction*)),this,SLOT(settingsSlot(QAction*)));
    connect(executeMenu,SIGNAL(triggered(QAction*)),this,SLOT(executeSlot(QAction*)));
    connect(helpMenu,SIGNAL(triggered(QAction*)),this,SLOT(helpSlot(QAction*)));
   }

void    MainWindow::addMessage(QString message,bool running)
{
    if(!running)
        mainEdit->append(message);
    else
        if(lastEdit!=NULL)
            lastEdit->append(message);
}

void    MainWindow::unload()
{
    if(problemLoader!=NULL)
    {
        delete problemLoader;
        problemLoader= NULL;
    }
    if(methodLoader!=NULL)
    {
        delete methodLoader;
        methodLoader = NULL;
    }
    if(myStat!=NULL)
    {
        delete myStat;
        myStat = NULL;
    }
}

void    MainWindow::noProblemLoaded()
{
    showMessageBox(appName,tr("No problem loaded."));
}

void    MainWindow::noMethodLoaded()
{
    showMessageBox(appName,tr("No method loaded."));
}

void    MainWindow::loadSlot(QAction *action)
{
    if(action->text()=="PROBLEM")
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
        delete dialog;
    }
    else
    if(action->text()=="METHOD")
    {
        if(myProblem == NULL) noProblemLoaded();
        else
        {
            SelectMethodDialog *dialog = new
                SelectMethodDialog(
                    methodLoader->getMethodList(),
                        methodLoader,
                    this
                    );
            dialog->setModal(true);
            int d = dialog->exec();
            if(d==QDialog::Accepted)
            {
                methodName =dialog->getSelectedMethod();
                addMessage("Method loaded "+methodName);
                myMethod=methodLoader->getSelectedMethod(methodName);
            }
            delete dialog;
        }
    }
    else
    if(action->text()=="QUIT")
    {
        unload();
        qApp->exit(0);
    }
}

void    MainWindow::settingsSlot(QAction  *action)
{
    if(action->text()=="PROBLEM")
    {
        if(myProblem == NULL) noProblemLoaded();
        else
        {
            QJsonObject pt ;
            if(problemLoader->getSelectedProblemName()=="mlpproblem")
            {
                pt = ((MlpProblem *)myProblem)->getParams();
            }
            else
                if(problemLoader->getSelectedProblemName()=="rbfproblem")
                {
                    pt = ((RbfProblem *)myProblem)->getParams();

                }
                else pt = problemLoader->getParams();
            ParameterDialog *d = new ParameterDialog(
                pt,
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
            delete d;
        }
    }
    else
    if(action->text()=="METHOD")
    {
        if(myMethod == NULL) noMethodLoaded();
        else
        {

            ParameterList list = myMethod->getParameterList();
            ParameterDialog *d = new ParameterDialog(
                        list,
                        methodName,this);
            d->setModal(true);
            int v=d->exec();
            if(v == QDialog::Accepted)
            {
                list = d->getParameterList();
                QJsonObject params = list.getParams();
                QJsonDocument doc(params);
                QString strJson(doc.toJson(QJsonDocument::Compact));
                addMessage("Method params "+strJson);
                myMethod->setParams(params);
            }
            delete d;
        }
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
}

void    MainWindow::executeSlot(QAction  *action)
{
    if(action->text()=="RUN")
    {
        if(myProblem==NULL)
        {
            noProblemLoaded();
            return;
        }
        if(myMethod == NULL) noMethodLoaded();
        else
        {
            if(myProblem==NULL)
            {
                noProblemLoaded();
                return;
            }
            else
            {
            ntimes = 30;
            bool ok;
            int i = QInputDialog::getInt(this, tr("Enter number of times"),
                                         tr("Number of times:"), ntimes, 0, 100, 1, &ok);
            if(ok)
            {
                ntimes =i;
                if(myStat!=NULL) delete myStat;

                startRunning();
            }
            }
        }
    }
    else
    if(action->text()=="STATISTICS")
    {
        if(myStat==NULL) noMethodLoaded();
        else
            addMessage(myStat->getStatistics(),true);
    }
    else
    if(action->text()=="TEST")
    {
        if(myProblem == NULL) noProblemLoaded();
        else
            addMessage(""+ProblemLoader::getProblemReport(problemLoader->getSelectedProblemName(),
                    myProblem));
    }

}

void    MainWindow::startRunning()
{
    if(lastProblem==problemLoader->getSelectedProblemName()
            && lastMethod == methodName && lastEdit!=NULL)
    {
        ;
    }
    else
    {
        QTextEdit *runEdit = new QTextEdit;
        runEdit->setGeometry(mainEdit->geometry());

        mainTab->addTab(runEdit,""+
                    problemLoader->getSelectedProblemName()+
                    " Running "+methodName);
        runEdit->setReadOnly(true);

        lastEdit = runEdit;
        lastProblem = problemLoader->getSelectedProblemName();
        lastMethod = methodName;
        this->repaint();
        mainTab->setCurrentIndex(mainTab->count()-1);
    }

    menuBar()->setEnabled(false);

    EditLogger *logger = new EditLogger(lastEdit);
    myMethod->setMethodLogger(logger);

    myStat = new Statistics;
    for(int t=1;t<=ntimes;t++)
    {
        srand(randomSeed+t);
        myProblem->resetFunctionCalls();
        myMethod->setProblem(myProblem);
        QJsonObject p = problemLoader->getParams();
        myProblem->init(p);
        myMethod->solve();
        myStat->addProblem(myProblem);
    }
    endRunning();
}
void    MainWindow::endRunning()
{
    myMethod->getMethodLogger()->printMessage(myStat->getStatistics());
    menuBar()->setEnabled(true);

}


void    MainWindow::helpSlot(QAction *action)
{
    if(action->text()=="TEAM")
    {
        showMessageBox(appName,                             tr("Tsoulos Ioannis\n"
                                   "Charilogis Vasileios\n"
                                   "Kirou Glikeria\n")); }
}
void    MainWindow::closeEvent(QCloseEvent *event)
{

      unload();
}

void    MainWindow::showMessageBox(QString title,QString message)
{
    QMessageBox  msgBox;
    msgBox.setText(message);
    msgBox.setWindowTitle(title);

    msgBox.setStyleSheet(
        QString("QLabel{min-width:%1 px; min-height: %3px;font-size: 24px;font-weight: bold} QPushButton{ width:%2px;height:%4px font-size: 18px;}").
        arg(qd.width()/4).arg(qd.width()/6).arg(qd.height()/10).arg(qd.height()/12));

    // Get the palette of the QMessageBox to modify child widget colors
    QPalette  palette = msgBox.palette();

    // Set the background color of child widgets (QLabel)
    palette.setColor(QPalette::Window, QColor(200, 200, 105));      // Yellow background color
    msgBox.setPalette(palette);

    // Show the QMessageBox
    msgBox.exec();
}

MainWindow::~MainWindow()
{
    //unload();
}

