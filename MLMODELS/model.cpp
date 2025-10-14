#include "model.h"

Model::Model()
{
    trainDataset = NULL;
    testDataset = NULL;
    method=NULL;
    addParam(Parameter("model_trainfile","The trainfile used"));
    addParam(Parameter("model_testfile","The test file used"));
    QStringList yesno;
    yesno<<"false"<<"true";
    addParam(Parameter("model_smote",yesno[0],yesno,"Enable or disable smote"));
    QStringList format;
    format<<"data"<<"csv"<<"arff";
    addParam(Parameter("model_trainformat",format[0],format,"Train file format"));
    addParam(Parameter("model_testformat",format[0],format,"Test file format"));
    modelSeed=1;
    generator.seed(modelSeed);
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

double  Model::getOutput(double *x)
{
    return 0.0;
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
    QString format = getParam("model_trainformat").getValue();
    QString smote = getParam("model_smote").getValue();
    if(tr=="") return;
    if(trainDataset!=NULL) delete trainDataset;
    trainDataset = new Dataset(tr,format);
    if(smote=="true")
    {
        trainDataset->makeSmote();
    }
    xall = trainDataset->getAllXpoint();
}

void    Model::initParams(QJsonObject &pt)
{
    for(QString key: pt.keys())
    {
        QString value=pt[key].toString();
        setParam(key,value);
    }
}
void        Model::loadTestSet()
{
    QString tt = getParam("model_testfile").getValue();
    QString format = getParam("model_testformat").getValue();

    if(tt=="") return;
    if(testDataset!=NULL) delete testDataset;
    testDataset = new Dataset(tt,format);


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

void    Model::setTestSet(Dataset *tt)
{
    testDataset = tt;
    testDataset->makePatternClass();
}

void        Model::setTrainSet(Dataset *tr)
{
    trainDataset = tr;
    trainDataset->makePatternClass();
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

        double per = getOutput(xx.data());
        if(fabs(per)>=1e+10) return 1e+100;
        if(isnan(per)) return 1e+100;
        if(isinf(per)) return 1e+100;
        error+= (per-yy)*(per-yy);
    }
    return error;
}
/** kanei oti kai i getTrainError() alla gia to test set **/
double  Model::getTestError(Dataset *test)
{
    double error = 0.0;
    int icount = test->count();
    for(int i=0;i<test->count();i++)
    {
        Data xx = test->getXPoint(i);
        double yy = test->getYPoint(i);
        double per = getOutput(xx);
	if(fabs(per)>1e+10) 
	{
		icount--;
		continue;
	}
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

void    Model::setModelSeed(int seed)
{
    modelSeed = seed;
    generator.seed(modelSeed);

}

int     Model::getModelSeed() const
{
    return modelSeed;
}

void	Model::printConfusionMatrix(vector<double> &T,vector<double> &O,
                             vector<double> &precision,
                             vector<double> &recall)
{
    int i,j;
    int N=T.size();
    Data dclass = testDataset->getPatternClass();
    int nclass=dclass.size();
    precision.resize(nclass);
    recall.resize(nclass);
    int **CM;
    //printf("** CONFUSION MATRIX ** Number of classes: %d\n",nclass);
    CM=new int*[nclass];
    for(i=0;i<nclass;i++) CM[i]=new int[nclass];
        for(i=0;i<nclass;i++)
          for(j=0;j<nclass;j++) CM[i][j] = 0;

    for(i=0;i<N;i++) CM[(int)T[i]][(int)O[i]]++;
    for(i=0;i<nclass;i++)
    {
        double sum = 0.0;
        for(j=0;j<nclass;j++)
            sum+=CM[j][i];
        precision[i]=sum==0?-1:CM[i][i]/sum;
        sum = 0.0;
        for(j=0;j<nclass;j++)
            sum+=CM[i][j];
         recall[i]=sum==0?-1:CM[i][i]/sum;
    }
     for(i=0;i<nclass;i++)
     {
        for(j=0;j<nclass;j++)
        {
            //printf("%4d ",CM[i][j]);
        }
       //printf("\n");
       delete[] CM[i];
    }
    delete[] CM;
}

void    Model::getPrecisionRecall(double &avg_precision,
                                  double &avg_recall,double &avg_fscore)
{
    int count = testDataset->count();
    Data dclass = testDataset->getPatternClass();

    vector<double> T;
    vector<double> O;
    T.resize(count);
    O.resize(count);

    for(unsigned int i=0;i<count;i++)
    {
        Data pattern = testDataset->getXPoint(i);
        double testy = testDataset->getYPoint(i);
        double tempOut = getOutput(pattern);
        T[i]=testDataset->nearestClassIndex(testy);
        O[i]=testDataset->nearestClassIndex(tempOut);
    }
    vector<double> precision;
    vector<double> recall;
    vector<double> fscore;
    fscore.resize(dclass.size());
    avg_precision = 0.0, avg_recall = 0.0,avg_fscore=0.0;
    printConfusionMatrix(T,O,precision,recall);
    int icount1=dclass.size();
    int icount2=dclass.size();
    for(int i=0;i<dclass.size();i++)
    {
        if(precision[i]>=0)
        avg_precision+=precision[i];
        else icount1--;
        if(recall[i]>=0)
        avg_recall+=recall[i];
        else icount2--;
        fscore[i]=2.0*precision[i]*recall[i]/(precision[i]+recall[i]);
        avg_fscore+=fscore[i];
    }
    avg_precision/=icount1;
    avg_recall/=icount2;
    avg_fscore=2.0 * avg_precision * avg_recall/(avg_precision+avg_recall);
}

ParameterList Model::getParameterList()
{
    ParameterList list;
    for(int i=0;i<mparams.size();i++)
    {
        Parameter pt = mparams[i];
        list.addParam(pt);
    }
    return list;
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
