#include "fcmodel.h"

FcModel::FcModel()
{
    QStringList cmodel;
    cmodel<<"mlp"<<"rbf"<<"nnc"<<"rule";
    addParam(Parameter("fc_createmodel",cmodel[0],cmodel,"Feature construction model"));
    addParam(Parameter("fc_evaluatemode",cmodel[0],cmodel,"Feature evaluate model"));
    addParam(Parameter("fc_popcount",200,50,10000,"Population count"));
    addParam(Parameter("fc_popsize",100,10,1000,"FC chromosome size"));
    addParam(Parameter("fc_popgens",200,10,10000,"Maximum generations for FC"));
    addParam(Parameter("fc_popsrate",0.10,0.0,1.0,"FC selection rate"));
    addParam(Parameter("fc_popmrate",0.05,0.0,1.0,"FC mutation rate"));
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

void        FcModel::trainModel()
{
    //construct models
    //initiate methods from loaders
    //construct population
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
