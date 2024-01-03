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
    addParam(Parameter("rule_lsearchiters","10","Number of iters before local search"));
    addParam(Parameter("rule_lsearchitems","1","Number of items in local search"));
    addParam(Parameter("rule_lsearchmethod","bfgs","Available methods: bfgs,lbfgs,adam,genetic"));
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
                      program);

 pop->setSelectionRate(getParam("rule_popsrate").getValue().toDouble());
 pop->setMutationRate(getParam("rule_popmrate").getValue().toDouble());
 pop->setLocalMethod(GELOCAL_SIMAN);


 int gens = getParam("rule_popgens").getValue().toInt();
 //if(trialProblem==NULL)  trialProblem = new MlpProblem();
 //trialProblem->disableRemoveData();
 //trialProblem->setTrainSet(trainDataset);
 int LI,LC;
 LI= getParam("rule_lsearchiters").getValue().toInt();
 LC= getParam("rule_lsearchitems").getValue().toInt();

 for(int g=1;g<=gens;g++)
 {
     pop->nextGeneration();
     printf(" generation = %d best value= %20.10lg\n",g,pop->getBestFitness());
     if(g%LI==0)
     {
         for(int i=1;i<=LC;i++)
         {
             int pos = rand() % pop->getCount();

             localSearchItem(pos);
         }
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
