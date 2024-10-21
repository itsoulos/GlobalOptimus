#include "methodloader.h"

MethodLoader::MethodLoader()
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
    methodName<<"Armadillo";
    method<<new OFAlgorithm;
    methodName<<"Ofa";
    method<<new GWOoptimizer;
    methodName<<"Gwo";
    method<<new EGO;
    methodName<<"Ego";
    method<<new HybridMethod;
    methodName<<"HybridMethod";
    method<<new WOA;
    methodName<<"Woa";
    method<<new MEWOA;
    methodName<<"MeWoa";
    method<<new UserMethod;
    methodName<<"UserMethod";
}

QStringList  MethodLoader::getMethodParamsNames(QString name) const
{
    QStringList list;
    if(!methodName.contains(name))
        return list;
    int index = methodName.indexOf(name);
    return method[index]->getParameterNames();
}


QStringList MethodLoader::getMethodList() const
{
    return methodName;
}

Optimizer   *MethodLoader::getSelectedMethod(QString name)
{
    if(!methodName.contains(name))
        return NULL;
    int index = methodName.indexOf(name);
    return method[index];
}

QJsonObject MethodLoader::getMethodParams(QString name) const
{
    int index = methodName.indexOf(name);
    return method[index]->getParams();
}

MethodLoader::~MethodLoader()
{
    for(int i=0;i<method.size();i++)
        delete method[i];
}
