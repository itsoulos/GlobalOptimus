#include <MLMODELS/rulemodel.h>

RuleModel::RuleModel()
{
    program = NULL;
    pop =   NULL;
    addParam(Parameter("rule_popcount","200","Population count"));
    addParam(Parameter("rule_popsize","100","RULER chromosome size"));
    addParam(Parameter("rule_popgens","200","Maximum generations for RULER"));
    addParam(Parameter("rule_popsrate","0.10","RULER selection rate"));
    addParam(Parameter("rule_popmrate","0.05","RULER mutation rate"));
    addParam(Parameter("rule_poplrate","0.00","RULER local search rate"));
    addParam(Parameter("rule_lsearchmethod","bfgs","Available methods: bfgs,lbfgs,adam,genetic,de"));
}

double  RuleModel::getOutput(Data &x)
{
 return program->getOutput(x.data());
}

void        RuleModel::localSearchItem(int pos)
{
}

void        RuleModel::trainModel()
{

 loadTrainSet();
 loadTestSet();

 if(trainDataset == NULL) return;
 if(pop!=NULL) delete pop;
 if(program!=NULL) delete program;
 program = new RuleProgram(trainDataset->dimension(),this);
 program->makeGrammar();


 pop = new Population(getParam("rule_popcount").getValue().toInt(),
                      getParam("rule_popsize").getValue().toInt(),
                      program,getModelSeed());

 pop->setSelectionRate(getParam("rule_popsrate").getValue().toDouble());
 pop->setMutationRate(getParam("rule_popmrate").getValue().toDouble());
 pop->setLocalSearchRate(getParam("rule_poplrate").getValue().toDouble());
 QString method=getParam("rule_lsearchmethod").getValue();
 if(method == "none")
    pop->setLocalMethod(GELOCAL_NONE);
 else
 if(method == "crossover")
    pop->setLocalMethod(GELOCAL_CROSSOVER);
 else
 if(method=="mutate")
     pop->setLocalMethod(GELOCAL_MUTATE);
 else
 if(method=="bfgs")
     pop->setLocalMethod(GELOCAL_BFGS);
 else
 if(method=="siman")
     pop->setLocalMethod(GELOCAL_SIMAN);
 else
     if(method=="de")
     pop->setLocalMethod(GELOCAL_DE);


 int gens = getParam("rule_popgens").getValue().toInt();
 //if(trialProblem==NULL)  trialProblem = new MlpProblem();
 //trialProblem->disableRemoveData();
 //trialProblem->setTrainSet(trainDataset);


 for(int g=1;g<=gens;g++)
 {
     pop->nextGeneration();
     int redo =0;
     vector<int> genome = pop->getBestGenome();

    
     if(g%50==0)
     {
        printf(" generation = %d best expression = %s best value= %20.10lg\n",g,
            program->printRandomProgram(genome,redo).c_str(),
            pop->getBestFitness());
            pop->evaluateBestFitness();
        printf("Errors %lf %lf \n",getTestError(),getClassTestError());
     }
 }
 pop->evaluateBestFitness();

}

RuleModel::~RuleModel()
{
 if(program!=NULL)
     delete program;
 if(pop!=NULL)
     delete pop;
}
