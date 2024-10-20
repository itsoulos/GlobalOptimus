#include "model.h"

Model::Model()
{
    trainDataset = NULL;
    testDataset = NULL;
    method=NULL;
    addParam(Parameter("model_trainfile","","The trainfile used"));
    addParam(Parameter("model_testfile","","The test file used"));
}

void    Model::trainModel()
{
    method->setProblem(dynamic_cast<Problem*>(this));

    if(method!=NULL)
        method->solve();
}


void    Model::testModel(double &trainError,double &testError,double &classError)
{
    if(method!=NULL)
    {
       Problem *p=dynamic_cast<Problem*>(this);
       Data xx = p->getBestx();
       QJsonObject values = p->done(xx);
       trainError=values["trainError"].toDouble();
       testError=values["testError"].toDouble();
       classError=values["classError"].toDouble();
    }
    else
    {
        trainError = getTrainError();
        testError  = getTestError();
        classError = getClassTestError();
    }
}

void    Model::initModel()
{

}
void   Model::setOptimizer(Optimizer *p)
{
    method = p;
}

void        Model::loadTrainSet()
{
    QString tr = getParam("model_trainfile").getValue();
    if(tr=="") return;
    if(trainDataset!=NULL) delete trainDataset;
    trainDataset = new Dataset(tr);
    xall = trainDataset->getAllXpoint();
}

void        Model::loadTestSet()
{
    QString tt = getParam("model_testfile").getValue();
    if(tt=="") return;
    if(testDataset!=NULL) delete testDataset;
    testDataset = new Dataset(tt);
}

double      Model::getTestError()
{
    return getTestError(testDataset);
}

double      Model::getClassTestError()
{
    return getClassTestError(testDataset);
}

double      Model::getDistance(Data &x1,Data &x2)
{
    double sum = 0.0;
    for(int i=0;i<x1.size();i++) sum+=(x1[i]-x2[i])*(x1[i]-x2[i]);
    return sqrt(sum);
}

void        Model::setTrainSet(Dataset *tr)
{
    trainDataset = tr;
    xall = trainDataset->getAllXpoint();
}

double  Model::getTrainError()
{
    double error = 0.0;
    int xallsize  = xall.size();
    for(int i=0;i<xallsize;i++)
    {
        Data xx = xall[i];

        double yy = trainDataset->getYPoint(i);
        double per = getOutput(xx);

        error+= (per-yy)*(per-yy);
    }

    return error;
}
/** kanei oti kai i getTrainError() alla gia to test set **/
double  Model::getTestError(Dataset *test)
{
    double error = 0.0;
    for(int i=0;i<test->count();i++)
    {
        Data xx = test->getXPoint(i);
        double yy = test->getYPoint(i);
        double per = getOutput(xx);
        error+= (per-yy)*(per-yy);
    }
    return error;
}

/** edo epistrefo to classification sfalma sto test set **/
double  Model::getClassTestError(Dataset *test)
{
    double error = 0.0;
    for(int i=0;i<test->count();i++)
    {
        Data xx = test->getXPoint(i);
        double realClass = test->getClass(i);
        double per = getOutput(xx);
        double estClass = test->getClass(per);
        /** an apexei i pragmatiki klasi apo tin ypologismeni
         *  pano apo 10^-5, tote exoume sfalma **/
        error+= (fabs(estClass - realClass)>1e-5);
    }
    /** to metatrepoume se pososto **/
    return error*100.0/test->count();
}
void        Model::disableRemoveData()
{
    noRemoveData=true;
}

Dataset     *Model::getTrainDataset()
{
    return trainDataset;
}

Model::~Model()
{
if(noRemoveData) return;
    if(trainDataset!=NULL)
    {
        delete trainDataset;
        trainDataset = NULL;
        }
    if(testDataset!=NULL)
    {
        delete testDataset;
        testDataset = NULL;
        }
}
