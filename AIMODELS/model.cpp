#include "model.h"

Model::Model()
{
    trainDataset = NULL;
    testDataset = NULL;
}

double  Model::getTrainError()
{
    double error = 0.0;
    for(int i=0;i<trainDataset->count();i++)
    {
        Data xx = trainDataset->getXPoint(i);
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

Model::~Model()
{
    if(trainDataset!=NULL)
        delete trainDataset;
    if(testDataset)
        delete testDataset;
}
