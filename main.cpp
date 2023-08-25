#include <QCoreApplication>
# include <PROBLEMS/rastriginproblem.h>
# include <PROBLEMS/test2nproblem.h>
# include <METHODS/gradientdescent.h>
# include <METHODS/multistart.h>
# include <METHODS/genetic.h>
# include <METHODS/bfgs.h>


QVector<Optimizer*> method;
QStringList methodName;
QVector<QStringList> methodParams;
Problem *mainProblem = NULL;
QVector<Parameter> mainParams;
QJsonObject problemParams;

QString header="==========================================================================";

void printOption(QString fullName,
                 QString description,QString value)
{
    QString s=QString("--%1 %2 %3 %4").arg(fullName,-27).
                arg(description,-35).arg("Default value:",-15).arg(value,-10);
    qDebug().noquote()<<s;
}
void makeMainParams()
{
    mainParams<<Parameter("help","","Show help screen");
    mainParams<<Parameter("opt_method","Multistart","Used Optimization method");
    mainParams<<Parameter("opt_problem","rastrigin","Used Optimization problem");
    mainParams<<Parameter("opt_seed","1","Random Seed");
    mainParams<<Parameter("opt_threads","1","Number of threads");
}
void loadMethods()
{
    method<<new GradientDescent;
    methodName<<"GradientDescent";
    method<<new Bfgs;
    methodName<<"Bfgs";
    method<<new Multistart;
    methodName<<"Multistart";
    method<<new Genetic;
    methodName<<"Genetic";
    for(int i=0;i<method.size();i++)
    methodParams<<method[i]->getParameterNames();
}

void loadProblem()
{
    QString problemName = "";
    for(int i=0;i<mainParams.size();i++)
    {
        if(mainParams[i].getName()=="opt_problem")
        {
            problemName = mainParams[i].getValue();
            break;
        }
    }
    if(problemName=="rastrigin")
        mainProblem = new RastriginProblem();
    else
        if(problemName=="test2n")
        mainProblem = new Test2nProblem();
              mainProblem->init(problemParams);

}

void unloadMethods()
{
    for(int i=0;i<method.size();i++)
        delete method[i];
}

void unloadProblem()
{
    if(mainProblem!=NULL)
        delete mainProblem;
}


void    printHelp()
{
    qDebug().noquote()<<"MAIN PARAMETERS";
    qDebug()<<header;
    for(int i=0;i<mainParams.size();i++)
    {
        Parameter pt = mainParams[i];
        printOption(pt.getName(),pt.getHelp(),pt.getValue());
    }
    for(int i = 0;i<method.size();i++)
    {
        qDebug()<<"METHOD: "<<methodName[i];
        qDebug()<<header;
        for(int j=0;j<methodParams[i].size();j++)
        {
            Parameter pt = method[i]->getParam(methodParams[i][j]);
            printOption(pt.getName(),pt.getHelp(),pt.getValue());
        }
        qDebug()<<header;

    }
    qApp->exit(0);
    exit(0);
}

void checkParameter(
    QString name,QString value,
    QString fullName,
    QString &parameter)
{
    if(name == fullName)
        parameter = value;
}

void error(QString message)
{
    qDebug().noquote()<<QString("Fatal Error: %1").arg(message,40)<<endl;
    qApp->exit(0);
    exit(0);
}

void parseCmdLine(QStringList args)
{
    QString lastParam="";
    for(int i=1;i<args.size();i++)
    {

        if(args[i]=="--help") printHelp();
        QStringList sp=args[i].split("=");
        QString name = sp[0];
        sp[0]=sp[0].replace("--","");
        QString value = sp[1];
        if(value=="")
        {
            error(QString("Param %1 is empty.").arg(value));
        }
        bool found = false;
        for(int j=0;j<mainParams.size();j++)
        {
            Parameter pt = mainParams[j];
            if(pt.getName()==sp[0])
            {
                mainParams[j].setValue(sp[1]);
                found = true;
                break;
            }
        }
        if(!found)
        {
            for(int j=0;j<method.size();j++)
            {
                if(found) break;
                for(int k=0;k<methodParams[j].size();k++)
                {
                    Parameter pt = method[j]->getParam(methodParams[j][k]);
                    if(pt.getName()==sp[0])
                    {
                        method[j]->setParam(sp[0],sp[1]);
                        found = true;
                    }
                }
            }
        }
        if(!found)
        {
            problemParams[sp[0]]=sp[1];
        }
    }
}

void runMethod()
{
    QString mname = "";
    int index =-1;
    for(int i=0;i<mainParams.size();i++)
    {
        if(mainParams[i].getName()=="opt_method")
        {
            mname = mainParams[i].getValue();
            for(int j=0;j<methodName.size();j++)
            {
                if(methodName[j]==mname)
                {
                    index = j;
                    break;
                }
            }
            break;
        }
    }
    method[index]->setProblem(mainProblem);
    method[index]->solve();
}
int main(int argc, char *argv[])
{
    QCoreApplication app(argc,argv);
    setlocale(LC_ALL,"C");
    makeMainParams();
    loadMethods();
    parseCmdLine(app.arguments());
    loadProblem();
    runMethod();
    unloadMethods();
    unloadProblem();
    return 0;
}
