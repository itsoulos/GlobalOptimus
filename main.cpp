#include <QCoreApplication>

# include <METHODS/gradientdescent.h>
# include <METHODS/multistart.h>
# include <METHODS/genetic.h>
# include <METHODS/bfgs.h>
# include <METHODS/lbfgs.h>
# include <METHODS/differentialevolution.h>
# include <METHODS/ipso.h>
# include <METHODS/neuralminimizer.h>
# include <METHODS/parallelde.h>
# include <METHODS/parallelpso.h>
# include <METHODS/neldermead.h>
# include <METHODS/adam.h>
# include <METHODS/armadillo1.h>
# include <METHODS/simanmethod.h>
# include <METHODS/usermethod.h>
# include <METHODS/ofalgorithm.h>
# include <METHODS/gwooptimizer.h>
# include <OPTIMUS/statistics.h>
#ifdef OPTIMUS_ARMADILLO
    # include <MLMODELS/functionalrbf.h>
#endif
# include <OPTIMUS/problemloader.h>
# include <OPTIMUS/methodloader.h>
# include <QVector>
# include <QDebug>
# include <QStringList>
void error(QString message);
//QVector<Optimizer*> method;
QStringList methodName;
QVector<QStringList> methodParams;
Problem *mainProblem = NULL;
QVector<Parameter> mainParams;
QJsonObject problemParams;
MethodLoader *methodLoader = NULL;
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
    mainParams<<Parameter("opt_method","Genetic","Used Optimization method");
    mainParams<<Parameter("opt_problem","test2n","Used Optimization problem");
    mainParams<<Parameter("opt_seed","1","Random Seed");
    mainParams<<Parameter("opt_iters","30","Number of iterations");
    mainParams<<Parameter("opt_threads","1","Number of threads");
}
void loadMethods()
{
    methodLoader = new MethodLoader();
    methodName = methodLoader->getMethodList();
    for(int i=0;i<methodName.size();i++)
        methodParams<<methodLoader->getMethodParamsNames(methodName[i]);

}

void loadProblem()
{
    QString problemName = "";
    bool found = false;
    for(int i=0;i<mainParams.size();i++)
    {
        if(mainParams[i].getName()=="opt_problem")
        {
            problemName = mainParams[i].getValue();
            break;
        }
    }
    mainProblem = ProblemLoader::getProblemFromName(problemName);
    if(mainProblem == NULL) 
	    error("Problem not found!");
    mainProblem->init(problemParams);

}

void unloadMethods()
{
    delete methodLoader;
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
    for(int i = 0;i<methodName.size();i++)
    {
        qDebug()<<"METHOD: "<<methodName[i];
        qDebug()<<header;
        for(int j=0;j<methodParams[i].size();j++)
        {
            Parameter pt =
                methodLoader->getSelectedMethod(methodName[i])->getParam(methodParams[i][j]);
            if(pt.getName().startsWith("opt_")) continue;
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
	printf("Fatal error %s \n",message.toStdString().c_str());
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
            for(int j=0;j<methodName.size();j++)
            {
                for(int k=0;k<methodParams[j].size();k++)
                {
                    Parameter pt =
                        methodLoader->getSelectedMethod(methodName[j])->getParam(methodParams[j][k]);
                    if(pt.getName()==sp[0])
                    {
                         methodLoader->getSelectedMethod(methodName[j])->setParam(sp[0],sp[1]);
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
    methodLoader->getSelectedMethod(methodName[index])->setProblem(mainProblem);
    methodLoader->getSelectedMethod(methodName[index])->solve();
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
