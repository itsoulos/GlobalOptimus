#include <QCoreApplication>

# include <MLMODELS/mlpproblem.h>
# include <MLMODELS/rbfproblem.h>
# include <MLMODELS/gdfmodel.h>
# include <MLMODELS/nncmodel.h>
# include <MLMODELS/rulemodel.h>


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
# include <METHODS/simanmethod.h>
# include <METHODS/usermethod.h>
# include <METHODS/armadillo1.h>
# include <METHODS/gwooptimizer.h>

# include <OPTIMUS/statistics.h>
#ifdef OPTIMUS_ARMADILLO
    # include <MLMODELS/functionalrbf.h>
#endif

# include <QVector>
# include <QDebug>
# include <QStringList>
void error(QString message);
QVector<Optimizer*> method;
QStringList methodName;
QVector<QStringList> methodParams;
Model *mainModel = NULL;
QVector<Parameter> mainParams;
QJsonObject problemParams;
QString modelName="";
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
    mainParams<<Parameter("opt_model","mlp","Model name. Values: mlp,rbf,gdf,nnc");
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
    method<<new Lbfgs;
    methodName<<"Lbfgs";
    method<<new Multistart;
    methodName<<"Multistart";
    method<<new Genetic;
    methodName<<"Genetic";
    method<<new DifferentialEvolution;
    methodName<<"DifferentialEvolution";
    method<<new iPso;
    methodName<<"iPso";
    method<<new NeuralMinimizer;
    methodName<<"NeuralMinimizer";
    method<<new ParallelDe;
    methodName<<"ParallelDe";
    method<<new ParallelPso;
    methodName<<"ParallelPso";
    method<<new NelderMead;
    methodName<<"NelderMead";
    method<<new Adam;
    methodName<<"Adam";
    method<<new SimanMethod;
    methodName<<"Simman";
    method<<new Armadillo1;
    methodName<<"armadillo1";
    method<<new GWOoptimizer;
    methodName<<"Gwo";
    method<<new UserMethod;
    methodName<<"UserMethod";
    for(int i=0;i<method.size();i++)
    methodParams<<method[i]->getParameterNames();
}


void loadProblem()
{

    for(int i=0;i<mainParams.size();i++)
    {
        if(mainParams[i].getName()=="opt_model")
        {
            modelName = mainParams[i].getValue();
            break;
        }
    }
    if(modelName == "mlp")
        mainModel = new MlpProblem();
    else
    if(modelName == "rbf")
        mainModel = new RbfProblem();
    else
    if(modelName == "gdf")
        mainModel = new GdfModel();
    else
    if(modelName == "nnc")
        mainModel = new NNCModel();
    else
    if(modelName == "rule")
        mainModel = new RuleModel();
   // else
   // if(modelName =="frbf")
     //  mainModel = new FunctionalRbf();
    QStringList modelParams = mainModel->getParameterNames();
    for(int i=0;i<problemParams.keys().size();i++)
    {
        QString pname = problemParams.keys().at(i);
        for(int j=0;j<modelParams.size();j++)
        {
            if(pname == modelParams[j])
            {
                mainModel->setParam(pname,problemParams[pname].toString());
            }
        }
    }
    if(modelName=="mlp" || modelName=="rbf" ||modelName == "frbf")
    {
       if(modelName == "mlp")
       {
        MlpProblem *p = (MlpProblem *)mainModel;
        p->init(problemParams);
       }
       else
      if(modelName == "frbf")
      {
      //    FunctionalRbf *p=(FunctionalRbf *)mainModel;
      //    p->init(problemParams);
      }
      else
       {
        RbfProblem *p = (RbfProblem *)mainModel;
        p->init(problemParams);
       }
    }

}

void unloadMethods()
{
    for(int i=0;i<method.size();i++)
        delete method[i];
}

void unloadProblem()
{
    if(mainModel!=NULL)
        delete mainModel;
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
    //trainModel
    if(modelName=="mlp" || modelName=="rbf" || modelName =="frbf")
    {
        if(modelName=="mlp")
        {
        MlpProblem *mainProblem = (MlpProblem*)mainModel;
        method[index]->setProblem(mainProblem);
        method[index]->solve();
        }
        else
        if(modelName=="rbf")
        {
        RbfProblem *mainProblem = (RbfProblem*)mainModel;
        method[index]->setProblem(mainProblem);
        method[index]->solve();

        }
        else
        if(modelName == "frbf")
        {
           // FunctionalRbf *mainProblem = (FunctionalRbf *)mainModel;
           // method[index]->setProblem(mainProblem);
           // method[index]->solve();
        }

    }
    else
    if(modelName=="gdf")
    {
        GdfModel *p = (GdfModel *)mainModel;
        p->trainModel();
    }
    else
    if(modelName == "nnc")
    {
        NNCModel *p=(NNCModel *)mainModel;
        p->trainModel();
    }
    else
    if(modelName == "rule")
    {
        RuleModel *p=(RuleModel *)mainModel;
        p->trainModel();
    }
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
    int times = getIters();
    double average_train_error  = 0.0;
    double average_test_error   = 0.0;
    double average_class_error  = 0.0;

    for(int t=1;t<=times;t++)
    {
        srand(t);
        loadProblem();
        runMethod();
        if(modelName == "mlp" ||
                modelName=="rbf"||
                modelName == "frbf")
        {
        Data xx ;
        if(modelName == "mlp")
            xx=((MlpProblem *)mainModel)->getBestx();
        else
        if(modelName=="rbf")
            xx=((RbfProblem *)mainModel)->getBestx();
      //  else
      //  xx=((FunctionalRbf *)mainModel)->getBestx();
        QJsonObject values;
        if(modelName == "mlp")
            values=((MlpProblem *)mainModel)->done(xx);
        else
        if(modelName == "rbf")
            values = ((RbfProblem *)mainModel)->done(xx);
        //else
        //values=((FunctionalRbf *)mainModel)->done(xx);
        average_train_error+=values["trainError"].toDouble();
        average_test_error+=values["testError"].toDouble();
        average_class_error+=values["classError"].toDouble();
        }
        else
        {
            average_train_error+=mainModel->getTrainError();
            average_test_error+=mainModel->getTestError();
            average_class_error+=mainModel->getClassTestError();
        }
        printf("TRAINING... TRAIN[%2d]=%10.5lf TEST[%2d]=%10.5lf CLASS[%2d]=%5.2lf%%\n",
             t,average_train_error/t,
             t,average_test_error/t,
               t, average_class_error/t);
        unloadProblem();

    }
    printf("Average Train Error: %20.10lg\n",average_train_error/times);
    printf("Average Test  Error: %20.10lg\n",average_test_error/times);
    printf("Average Class Error: %20.10lg%%\n",average_class_error/times);

    unloadMethods();
    return 0;
}
