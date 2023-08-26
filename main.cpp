#include <QCoreApplication>
# include <PROBLEMS/rastriginproblem.h>
# include <PROBLEMS/test2nproblem.h>
# include <PROBLEMS/bf1.h>
# include <PROBLEMS/bf2.h>
# include <PROBLEMS/branin.h>
# include <PROBLEMS/camel.h>
# include <PROBLEMS/cm.h>
# include <PROBLEMS/diffpower.h>
# include <PROBLEMS/easom.h>
# include <PROBLEMS/elp.h>
# include <PROBLEMS/exp.h>
# include <PROBLEMS/gkls250.h>
# include <METHODS/gradientdescent.h>
# include <METHODS/multistart.h>
# include <METHODS/genetic.h>
# include <METHODS/bfgs.h>
# include <METHODS/differentialevolution.h>
# include <OPTIMUS/statistics.h>


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
    mainParams<<Parameter("opt_method","DifferentialEvolution","Used Optimization method");
    mainParams<<Parameter("opt_problem","gkls250","Used Optimization problem");
    mainParams<<Parameter("opt_seed","1","Random Seed");
    mainParams<<Parameter("opt_iters","30","Number of iterations");
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
    method<<new DifferentialEvolution;
    methodName<<"DifferentialEvolution";
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
    else
        if(problemName=="bf1")
        mainProblem  = new Bf1();
    else
        if(problemName=="bf2")
        mainProblem = new Bf2();
    else
        if(problemName=="branin")
        mainProblem = new Branin();
    else
        if(problemName=="camel")
        mainProblem = new Camel();
    else
        if(problemName=="diffpower")
        mainProblem = new DiffPower();
    else
        if(problemName == "easom")
        mainProblem = new Easom();
    else
        if(problemName == "elp")
        mainProblem  = new Elp();
    else
        if(problemName == "exp")
        mainProblem = new Exp();
    else
       if(problemName == "gkls250")
          mainProblem = new Gkls250();
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

int getIters()
{
    for(int i=0;i<mainParams.size();i++)
    {
        Parameter p = mainParams[i];
        if(p.getName()=="opt_iters")
            return p.getValue().toInt();
    }
    return 0;
}
int main(int argc, char *argv[])
{
    QCoreApplication app(argc,argv);
    setlocale(LC_ALL,"C");
    makeMainParams();
    loadMethods();
    parseCmdLine(app.arguments());


    unloadProblem();
    Statistics stat;
    int times = getIters();

    for(int t=1;t<=times;t++)
    {
        srand(t);
        loadProblem();
        runMethod();
        stat.addProblem(mainProblem);
        unloadProblem();
    }


    stat.printStatistics();
    unloadMethods();
    return 0;
}
