#include "fcmodel.h"
# include <MLMODELS/mlpproblem.h>
# include <MLMODELS/rbfproblem.h>
# include <MLMODELS/nncmodel.h>
# include <MLMODELS/gdfmodel.h>
# include <MLMODELS/rulemodel.h>
FcModel::FcModel()
{
    QStringList cmodel;
    cmodel<<"mlp"<<"rbf"<<"nnc"<<"rule"<<"gdf";
    addParam(Parameter("fc_createmodel",cmodel[0],cmodel,"Feature construction model"));
    addParam(Parameter("fc_evaluatemode",cmodel[0],cmodel,"Feature evaluate model"));
    addParam(Parameter("fc_popcount",200,50,10000,"Population count"));
    addParam(Parameter("fc_popsize",100,10,1000,"FC chromosome size"));
    addParam(Parameter("fc_popgens",200,1,10000,"Maximum generations for FC"));
    addParam(Parameter("fc_popsrate",0.10,0.0,1.0,"FC selection rate"));
    addParam(Parameter("fc_popmrate",0.05,0.0,1.0,"FC mutation rate"));
    addParam(Parameter("fc_features",1,1,100,"Number of constructed features"));
    addParam(Parameter("fc_createparams","","Parameters for construction model"));
    addParam(Parameter("fc_evaluateparams","","Parameters for evaluation model"));
    //addParam(Parameter("nnc_lsearchiters","10","Number of iters before local search"));
    //addParam(Parameter("nnc_lsearchitems","1","Number of items in local search"));
    //addParam(Parameter("nnc_lsearchrate",0.0,0.0,1.0,"Local search rate for nnc"));
    //addParam(Parameter("nnc_crossitems",100,10,10000,"Crossover items"));
    //QStringList methods;
    //methods<<"crossover"<<"mutate"<<"bfgs"<<"none";
    //addParam(Parameter("nnc_lsearchmethod",methods[0],methods,"Available methods: crossover,mutate,siman,bfgs,none"));

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
    return ret;
}

void        FcModel::trainModel()
{
    //construct models
    QString model1 = getParam("fc_createmodel").getValue();
    createModel = getModelByName(model1);
    QString model2 = getParam("fc_evaluatemode").getValue();
    evaluateModel = getModelByName(model2);
    //initiate methods from loaders
    QString params1=getParam("fc_createparams").getValue();
    params1.replace("\"","");
    params1.replace("\\"," ");
    QVector<QStringList> listArray1=createModel->parseString(params1);
    QString params2=getParam("fc_evaluateparams").getValue();
    params2.replace("\"","");
    params2.replace("\\"," ");
    QVector<QStringList> listArray2=evaluateModel->parseString(params2);
    if(model1=="mlp" || model1=="rbf")
    {
        createLoader=new MethodLoader();
        QStringList paramNames=listArray1[0];
        QStringList paramValues=listArray1[1];
        int i1=paramNames.indexOf("opt_method");
        if(i1!=-1)
        {
            createMethod=createLoader->getSelectedMethod(paramValues[i1]);
        }
        else createMethod=createLoader->getSelectedMethod("Bfgs");
        if(model1=="mlp" || model1=="rbf")
        {
            createModel->setOptimizer(createMethod);
        }
        //set method params
        for(int i=0;i<paramNames.size();i++)
        {
            createMethod->setParam(paramNames[i],paramValues[i]);
        }
    }
    if(model2=="mlp" || model2=="rbf")
    {
        evaluateLoader =new MethodLoader();
        QStringList paramNames=listArray2[0];
        QStringList paramValues=listArray2[1];
        int i1=paramNames.indexOf("opt_method");
        if(i1!=-1)
        {
            evaluateMethod=evaluateLoader->getSelectedMethod(paramValues[i1]);
        }
        else evaluateMethod=evaluateLoader->getSelectedMethod("Bfgs");
        if(model2=="mlp" || model2=="rbf")
        {
               evaluateModel->setOptimizer(evaluateMethod);
        }
        //set method params
        for(int i=0;i<paramNames.size();i++)
        {
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

    pop = new Population(getParam("fc_popcount").getValue().toInt(),
                     getParam("fc_popsize").getValue().toInt(),
                     program);

    pop->setSelectionRate(getParam("fc_popsrate").getValue().toDouble());
    pop->setMutationRate(getParam("fc_popmrate").getValue().toDouble());
    int gens = getParam("fc_popgens").getValue().toInt();
    for(int g=1;g<=gens;g++)
    {
        pop->nextGeneration();
        printf("Fc Generation[%4d]=%lf \n",g,pop->getBestFitness());
    }
    pop->evaluateBestFitness();
}

void  FcModel::testModel(double &trainError,
                         double &testError,
                         double &classError)
{
    Mapper *mapper = program->getMapper();
    Dataset *mappedDataset =new Dataset();
    mapper->mapDataset(trainDataset,mappedDataset);
    evaluateModel->setTrainSet(mappedDataset);
    evaluateModel->initModel();
    evaluateModel->trainModel();

    Dataset *mappedTestSet = new Dataset();
    mapper->mapDataset(testDataset,mappedTestSet);
    evaluateModel->setTestSet(mappedTestSet);
    evaluateModel->testModel(trainError,testError,classError);

    delete mappedDataset;
    delete mappedTestSet;
}

FcModel::~FcModel()
{
    if(pop!=NULL)
    {
        delete pop;
        delete createLoader;
        delete createModel;
        delete evaluateLoader;
        delete evaluateModel;
        delete program;
    }
}
