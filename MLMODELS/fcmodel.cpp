#include "fcmodel.h"
# include <MLMODELS/mlpproblem.h>
# include <MLMODELS/rbfproblem.h>
# include <MLMODELS/nncmodel.h>
# include <MLMODELS/gdfmodel.h>
# include <MLMODELS/rulemodel.h>
# include <QDebug>
#ifdef OPTIMUS_ARMADILLO
# include <MLMODELS/functionalrbf.h>
#endif
FcModel::FcModel()
{
    QStringList cmodel;
    cmodel<<"mlp"<<"rbf"<<"nnc"<<"rule"<<"gdf"<<"frbf";
    addParam(Parameter("fc_createmodel",cmodel[0],cmodel,"Feature construction model"));
    addParam(Parameter("fc_evaluatemodel",cmodel[0],cmodel,"Feature evaluate model"));
    addParam(Parameter("fc_popcount",200,50,10000,"Population count"));
    addParam(Parameter("fc_popsize",100,10,1000,"FC chromosome size"));
    addParam(Parameter("fc_popgens",200,1,10000,"Maximum generations for FC"));
    addParam(Parameter("fc_popsrate",0.10,0.0,1.0,"FC selection rate"));
    addParam(Parameter("fc_popmrate",0.05,0.0,1.0,"FC mutation rate"));
    addParam(Parameter("fc_features",1,1,100,"Number of constructed features"));
    addParam(Parameter("fc_createparams","","Parameters for construction model"));
    addParam(Parameter("fc_evaluateparams","","Parameters for evaluation model"));
    QStringList yesNo;
    yesNo<<"no"<<"yes";
    addParam(Parameter("fc_balanceclass",yesNo[0],yesNo,"Enable or disable the error per class feature"));
    pop = NULL;
    createLoader=NULL;
    createModel=NULL;
    evaluateLoader=NULL;
    evaluateModel=NULL;
    program = NULL;
}

double  FcModel::getOutput(Data &x)
{
    return 0.0;
}

Model *getModelByName(QString model1)
{

    Model *ret=NULL;
    if(model1=="mlp")
    {
        ret = new MlpProblem();

    }
    else
    if(model1=="rbf")
    {
        ret = new RbfProblem();
    }
    else
    if(model1=="nnc")
    {
        ret = new NNCModel();
    }
    else
    if(model1=="rule")
    {
        ret = new RuleModel();
    }
    else
    if(model1=="gdf")
    {
        ret = new GdfModel();
    }
#ifdef OPTIMUS_ARMADILLO
    else
    if(model1=="frbf")
    {
        ret = new FunctionalRbf();
    }
#endif
    return ret;
}

void        FcModel::trainModel()
{
    //construct models
    QString model1 = getParam("fc_createmodel").getValue();
    if(createModel==NULL)
    	createModel = getModelByName(model1);
    createModel->disableRemoveData();
    createModel->setModelSeed(getModelSeed());
    QString model2 = getParam("fc_evaluatemodel").getValue();
    if(evaluateModel==NULL)
    	evaluateModel = getModelByName(model2);
    evaluateModel->setModelSeed(getModelSeed());
    //initiate methods from loaders
    QString params1=getParam("fc_createparams").getValue();
    params1.replace("\"","");
    params1.replace("\\"," ");
    QVector<QStringList> listArray1=createModel->parseString(params1);
    QString params2=getParam("fc_evaluateparams").getValue();
    params2.replace("\"","");
    params2.replace("\\"," ");
    QVector<QStringList> listArray2=evaluateModel->parseString(params2);
    if(model1=="mlp" || model1=="rbf" || model1=="frbf")
    {
        QStringList paramNames=listArray1[0];
        QStringList paramValues=listArray1[1];
        int i1=paramNames.indexOf("opt_method");
        if(i1!=-1)
        {
		if(createLoader==NULL)
       	    		createLoader=new MethodLoader();
        if(createMethod==NULL)
            createMethod=createLoader->getSelectedMethod(paramValues[i1]);
        }
        else createMethod = NULL;
        //else createMethod=createLoader->getSelectedMethod("Bfgs");

        if((model1=="mlp" || model1=="rbf" || model1=="frbf")
            && createLoader!=NULL)
        {
            createModel->setOptimizer(createMethod);
        }
        //set method params
        for(int i=0;i<paramNames.size();i++)
        {
            if(createMethod!=NULL)  createMethod->setParam(paramNames[i],paramValues[i]);
            createModel->setParam(paramNames[i],paramValues[i]);
        }
        createModel->setParam("model_trainfile",getParam("model_trainfile").getValue());
        createModel->setParam("model_testfile",getParam("model_testfile").getValue());
        createModel->initModel();
    }
    if(model2=="mlp" || model2=="rbf" || model2=="frbf")
    {
	    if(evaluateLoader==NULL)
        	evaluateLoader =new MethodLoader();
        QStringList paramNames=listArray2[0];
        QStringList paramValues=listArray2[1];
        int i1=paramNames.indexOf("opt_method");
        if(i1!=-1)
        {
            if(evaluateMethod==NULL)
                evaluateMethod=evaluateLoader->getSelectedMethod(paramValues[i1]);
        }
        if(model2=="mlp" || model2=="rbf" || model2=="frbf")
        {
               evaluateModel->setOptimizer(evaluateMethod);
        }
        //set method params
        for(int i=0;i<paramNames.size();i++)
        {
            if(evaluateMethod!=NULL)
            evaluateMethod->setParam(paramNames[i],paramValues[i]);
        }
    }
    loadTrainSet();
    loadTestSet();
    if(trainDataset == NULL)
    {
        return;
    }
    if(pop!=NULL) delete pop;
    if(program!=NULL) delete program;
    //construct population
    int nfeatures = getParam("fc_features").getValue().toInt();

    program = new FcProgram(trainDataset->dimension(),createModel,
                            trainDataset,
                            nfeatures);
    program->makeGrammar();
    program->setUseErrorPerClass(
        (getParam("fc_balanceclass").getValue()=="yes")?true:false
        );
    pop = new Population(getParam("fc_popcount").getValue().toInt(),
                     getParam("fc_popsize").getValue().toInt(),
                     program,getModelSeed());


    pop->setSelectionRate(getParam("fc_popsrate").getValue().toDouble());
    pop->setMutationRate(getParam("fc_popmrate").getValue().toDouble());
    int gens = getParam("fc_popgens").getValue().toInt();
 //   pop->setLocalSearchRate(0.001);
 //   pop->setLocalMethod(GELOCAL_MUTATE);

    for(int g=1;g<=gens;g++)
    {
        pop->nextGeneration();
        if(g%10==0)
            printf("FC Gen=%4d ERROR=%20.10lg\n",g,pop->getBestFitness());
    }
    pop->evaluateBestFitness();
}

void  FcModel::testModel(double &avg_trainError,
                         double &avg_testError,
                         double &avg_classError,
                        double &classErrorPerClass)
{
	vector<int> g = pop->getBestGenome();
	pop->fitness(g);
    Mapper *mapper = program->getMapper();
	avg_trainError = 0.0;
	avg_testError = 0.0;
	avg_classError = 0.0;
    const int iters=30;
    Dataset *mappedDataset =new Dataset();
    mapper->mapDataset(trainDataset,mappedDataset);
    evaluateModel->setTrainSet(mappedDataset);
    Dataset *mappedTestSet = new Dataset();
    mapper->mapDataset(testDataset,mappedTestSet);
    avgPrec=0.0,avgRec=0.0,avgF1=0.0;
    isTestRunning=true;
	for(int i=1;i<=iters;i++)
	{

    	evaluateModel->initModel();
    	evaluateModel->trainModel();

    	evaluateModel->setTestSet(mappedTestSet);
        double trainError  =0.0,testError=0.0,classError=0.0,classErrorPerClass=0.0;
        evaluateModel->testModel(trainError,testError,
                                 classError,classErrorPerClass);

        double x,y,z;
        getPrecisionRecall(x,y,z);
        avgPrec+=x;
        avgRec+=y;
        avgF1+=z;

        evaluateModel->disableRemoveData();
        avg_trainError+=trainError;
        avg_testError+=testError;
        avg_classError+=classError;

	}
    avgPrec/=iters;
    avgRec/=iters;
    avgF1/=iters;


    delete mappedTestSet;
    delete mappedDataset;


	avg_trainError/=iters;
	avg_testError/=iters;
	avg_classError/=iters;
    isTestRunning=false;
}


void    FcModel::getPrecisionRecall(double &avg_precision,
                                 double &avg_recall,double &avg_fscore)
{

    if(!isTestRunning)
    {
        avg_precision=avgPrec;
        avg_recall=avgRec;
        avg_fscore=avgF1;
        return;
    }
    Mapper *mapper = program->getMapper();
    Dataset *mappedTestSet = new Dataset();
    mapper->mapDataset(testDataset,mappedTestSet);
    int count = testDataset->count();
    Data dclass = testDataset->getPatternClass();

    vector<double> T;
    vector<double> O;
    T.resize(count);
    O.resize(count);

    for(unsigned int i=0;i<count;i++)
    {
        Data pattern = mappedTestSet->getXPoint(i);
        double testy = testDataset->getYPoint(i);
        double tempOut = evaluateModel->getOutput(pattern);
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

    double test=2.0 * avg_precision * avg_recall/(avg_precision+avg_recall);
    avg_fscore=test;
    delete mappedTestSet;
}

FcModel::~FcModel()
{
    if(pop!=NULL)
    {
        delete pop;
	if(createLoader!=NULL)
	{
        	delete createLoader;
	}
	if(createModel!=NULL)
        delete createModel;
	if(evaluateLoader!=NULL)
        delete evaluateLoader;
	if(evaluateModel!=NULL)
       delete evaluateModel;
	if(program!=NULL)
        delete program;
    }
}
