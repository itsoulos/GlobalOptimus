#include "problemloader.h"
ProblemLoader::ProblemLoader()
{
    myProblem = NULL;
    problemList<<"rastrigin"<<"test2n"<<"bf1"<<"bf2"
        <<"branin"<<"camel"<<"cm"<<"diffpower"<<"easom"<<"exp"
        <<"elp"<<"gkls250"<<"gkls2100"<<"gkls350"
       <<"goldstein"<<"griewank2"<<"griewank10"
       <<"hansen"<<"hartman3"<<"hartman6"<<"potential"
       <<"rosenbrock"<<"shekel5"<<"shekel7"<<"shekel10"
       <<"sinu"<<"test30n"<<"mlpproblem"<<"rbfproblem"
       <<"userproblem";
    myParams["dummy"]=1;
}

QStringList     ProblemLoader::getProblemList() const
{
    return problemList;
}

Problem         *ProblemLoader::loadProblem(QString name)
{
    if(myProblem!=NULL) delete myProblem;
    if(problemList.contains("name")==false)
        return NULL;
    if(name == "rastrigin")
        myProblem = new RastriginProblem();
    else
    if(name == "test2n")
        myProblem = new Test2nProblem();
    else
    if(name == "bf1")
        myProblem = new Bf1();
    else
    if(name == "bf2")
        myProblem = new Bf2();
    else
    if(name == "branin")
        myProblem = new Branin();
    else
    if(name == "camel")
        myProblem = new Camel();
    else
    if(name == "cm")
        myProblem = new Cm();
    else
    if(name == "diffpower")
        myProblem = new DiffPower();
    else
    if(name == "easom")
        myProblem = new Easom();
    else
    if(name == "exp")
        myProblem = new Exp();
    else
    if(name == "elp")
        myProblem = new Elp();
    else
    if(name == "gkls250")
        myProblem = new Gkls250();
    else
    if(name == "gkls2100")
        myProblem = new Gkls2100();
    else
    if(name == "gkls350")
        myProblem = new Gkls350();
    else
    if(name == "goldstein")
        myProblem = new Goldstein();
    else
    if(name == "griewank2")
        myProblem = new Griewank2();
    else
    if(name == "griewank10")
        myProblem = new Griewank10();
    else
    if(name == "hansen")
        myProblem = new Hansen();
    else
    if(name == "hartman3")
        myProblem = new Hartman3();
    else
    if(name == "hartman6")
        myProblem = new Hartman6();
    else
    if(name == "potential")
        myProblem = new Potential();
    else
    if(name == "rosenbrock")
        myProblem = new Rosenbrock();
    else
    if(name == "shekel5")
        myProblem = new Shekel5();
    else
    if(name == "shekel7")
        myProblem = new Shekel7();
    else
    if(name == "shekel10")
        myProblem = new Shekel10();
    else
    if(name == "sinu")
        myProblem = new Sinu();
    else
    if(name == "test30n")
        myProblem = new Test30n();
    else
    if(name == "mlpproblem")
        myProblem = new MlpProblem();
    else
    if(name == "rbfproblem")
        myProblem = new RbfProblem();
    else
    if(name == "userproblem")
        myProblem = new UserProblem();
    return myProblem;
}

void            ProblemLoader::setParams(QJsonObject &p)
{
    myParams= p;
}

QJsonObject     ProblemLoader::getParams() const
{
    return myParams;
}

ProblemLoader::~ProblemLoader()
{
    if(myProblem!=NULL)
        delete myProblem;
}
