#include "nncmodel.h"


NNCModel::NNCModel()
{
    program = NULL;
    pop =   NULL;
    parser = NULL;
    trialProblem = NULL;
    addParam(Parameter("nnc_popcount",200,50,10000,"Population count"));
    addParam(Parameter("nnc_popsize",100,10,1000,"NNC chromosome size"));
    addParam(Parameter("nnc_popgens",200,10,10000,"Maximum generations for NNC"));
    addParam(Parameter("nnc_popsrate",0.10,0.0,1.0,"NNC selection rate"));
    addParam(Parameter("nnc_popmrate",0.05,0.0,1.0,"NNC mutation rate"));
    addParam(Parameter("nnc_lsearchiters","5","Number of iters before local search"));
    addParam(Parameter("nnc_lsearchitems","5","Number of items in local search"));
    addParam(Parameter("nnc_lsearchrate",0.0,0.0,1.0,"Local search rate for nnc"));
    addParam(Parameter("nnc_crossitems",100,10,10000,"Crossover items"));
    QStringList methods;
    methods<<"crossover"<<"mutate"<<"bfgs"<<"none";
    addParam(Parameter("nnc_lsearchmethod",methods[0],methods,"Available methods: crossover,mutate,siman,bfgs,none"));
}

double  NNCModel::getOutput(Data &x)
{

    return program->Eval(x.data());
}

void    NNCModel::trainModel()
{
        loadTrainSet();
        loadTestSet();

        if(trainDataset == NULL) return;
        if(pop!=NULL) delete pop;
        if(program!=NULL) delete program;
        program = new NncProgram(trainDataset->dimension(),this);
        program->makeGrammar();

        pop = new Population(getParam("nnc_popcount").getValue().toInt(),
                         getParam("nnc_popsize").getValue().toInt(),
                         program,getModelSeed());

        pop->setSelectionRate(getParam("nnc_popsrate").getValue().toDouble());
        pop->setMutationRate(getParam("nnc_popmrate").getValue().toDouble());
        pop->setLocalSearchRate(getParam("nnc_lsearchrate").getValue().toDouble());
        pop->setCrossoverItems( getParam("nnc_crossitems").getValue().toInt());

    int gens = getParam("nnc_popgens").getValue().toInt();
    if(trialProblem==NULL)  trialProblem = new MlpProblem();
    trialProblem->disableRemoveData();
    trialProblem->setTrainSet(trainDataset);
    int LI,LC;
    LI= getParam("nnc_lsearchiters").getValue().toInt();
    LC= getParam("nnc_lsearchitems").getValue().toInt();
    if(parser!=NULL) delete parser;
    parser =new NeuralParser(trainDataset->dimension());
    QString Lmethod = getParam("nnc_lsearchmethod").getValue();
    if(Lmethod == "none")
        pop->setLocalMethod(GELOCAL_NONE);
    else
        if(Lmethod == "crossover")
        pop->setLocalMethod(GELOCAL_CROSSOVER);
    else
        if(Lmethod == "mutate")
        pop->setLocalMethod(GELOCAL_MUTATE);
    else
        if(Lmethod == "bfgs")
        pop->setLocalMethod(GELOCAL_BFGS);
    else
        pop->setLocalMethod(GELOCAL_SIMAN);
    pop->setLocalSearchGenerations(LI);
    pop->setLocalSearchItems(LC);
    for(int g=1;g<=gens;g++)
    {
        pop->nextGeneration();
        if(g%50==0)
        printf(" generation = %d best value= %20.10lg\n",g,pop->getBestFitness());
        /*if(g%LI==0)
            {
                for(int i=1;i<=LC;i++)
                {
                    int pos = rand() % pop->getCount();

                    localSearchItem(pos);
                }
        }*/
       }

    pop->evaluateBestFitness();
   // setParam("nnc_lsearchmethod","bfgs");
   // localSearchItem(0);
}

void        NNCModel::localSearchItem(int pos)
{
       QString Lmethod = getParam("nnc_lsearchmethod").getValue();
       if(Lmethod == "random")
       {
        pop->localSearch(pos);
        return;
       }
        vector<int> xx;
        double yy;
        pop->getChromosome(pos,xx,yy);
        if(fabs(yy)>1e+10) return;
        int redo = 0;
        string st = program->printRandomProgram(xx,redo);
        if(!program->Parse(st)) return;
        parser->makeVector(st);
        string pt = parser->print();

       Data w;
        parser->getWeights(w);
        trialProblem->resetFunctionCalls();
        trialProblem->setWeights(w);
        trialProblem->setDimension(w.size());
        Data xu,xl;
        xu.resize(w.size());
        xl.resize(w.size());
        for(int j=0;j<w.size();j++)
        {
            xl[j]=-4.0*fabs(w[j]);
            xu[j]= 4.0*fabs(w[j]);

        }
        trialProblem->setLeftMargin(xl);
        trialProblem->setRightMargin(xu);
        Optimizer *method=NULL;
        if(Lmethod == "bfgs")
            method=new Bfgs();
        else
        if(Lmethod == "lbfgs")
            method = new Lbfgs();
        else
        if(Lmethod == "adam")
            method = new Adam();
        else
        if(Lmethod == "genetic")
            method = new Genetic();
        method->setProblem(trialProblem);

        if(Lmethod == "bfgs")
            ((Bfgs *)method)->setPoint(w,yy);
        else
        if(Lmethod == "lbfgs")
            ((Lbfgs *)method)->setPoint(w,yy);
        else
        if(Lmethod=="adam")
            ((Adam *)method)->setPoint(w,yy);
        else
        if(Lmethod == "genetic")
            ((Genetic *)method)->setBest(w,yy);
        method->solve();
        w=trialProblem->getBestx();
        delete method;
        Converter con(w,w.size()/(trainDataset->dimension()+2),trainDataset->dimension());
        con.convert(xx);
        redo = 0;
        if(xx.size()>pop->getSize()) return;
        string ss = program->printRandomProgram(xx,redo);
        if(!program->Parse(ss)) return ;
        double newy = pop->fitness(xx);
        if(fabs(newy)<fabs(yy))
        {
            printf("Local Search[%lf]->%lf\n",yy,newy);
            pop->setChromosome(pos,xx,newy);
        }
}

NNCModel::~NNCModel()
{
if(program!=NULL)
        delete program;
    if(pop!=NULL)
        delete pop;
 if(parser!=NULL)
    delete parser;
     delete trialProblem;
}
