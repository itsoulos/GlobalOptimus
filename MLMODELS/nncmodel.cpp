#include "nncmodel.h"


NNCModel::NNCModel()
{
    program = NULL;
    pop =   NULL;
    parser = NULL;
    trialProblem = NULL;
    addParam(Parameter("nnc_popcount","200","Population count"));
    addParam(Parameter("nnc_popsize","100","NNC chromosome size"));
    addParam(Parameter("nnc_popgens","200","Maximum generations for NNC"));
    addParam(Parameter("nnc_popsrate","0.90","NNC selection rate"));
    addParam(Parameter("nnc_popmrate","0.05","NNC mutation rate"));
    addParam(Parameter("nnc_lsearchiters","10","Number of iters before local search"));
    addParam(Parameter("nnc_lsearchitems","1","Number of items in local search"));
    addParam(Parameter("nnc_lsearchmethod","bfgs","Available methods: bfgs,lbfgs,adam,genetic"));
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
                         program);

        pop->setSelectionRate(getParam("nnc_popsrate").getValue().toDouble());
        pop->setMutationRate(getParam("nnc_popmrate").getValue().toDouble());



    int gens = getParam("nnc_popgens").getValue().toInt();
    if(trialProblem==NULL)  trialProblem = new MlpProblem();
    trialProblem->disableRemoveData();
    trialProblem->setTrainSet(trainDataset);
    int LI,LC;
    LI= getParam("nnc_lsearchiters").getValue().toInt();
    LC= getParam("nnc_lsearchitems").getValue().toInt();
    if(parser!=NULL) delete parser;
    parser =new NeuralParser(trainDataset->dimension());
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

void        NNCModel::localSearchItem(int pos)
{


        vector<int> xx;
        double yy;
        pop->getChromosome(pos,xx,yy);
        if(fabs(yy)>1e+10) return;
        int redo = 0;
        string st = program->printRandomProgram(xx,redo);
        if(!program->Parse(st)) return;
        parser->makeVector(st);
        string pt = parser->print();
    QString Lmethod = getParam("nnc_lsearchmethod").getValue();

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
            xl[j]=-2.0*fabs(w[j]);
            xu[j]= 2.0*fabs(w[j]);

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
