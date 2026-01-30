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
    addParam(Parameter("nnc_steadystate","no","Enable or disable steady state"));
    addParam(Parameter("nnc_weightfactor",2.0,1.0,10.0,"The weight factor for local optimization"));
    QStringList yesno;
    yesno<<"no"<<"yes";
    addParam(Parameter("nnc_enablebound",yesno[0],yesno,"Enable (yes) or disable (no) the bounding solution for MLP"));
    QStringList methods;
    methods<<"crossover"<<"mutate"<<"bfgs"<<"none"<<"genetic"<<"mbfgs"<<"siman";
    addParam(Parameter("nnc_lsearchmethod",methods[0],methods,"Available methods: crossover,mutate,siman,bfgs,none"));

    addParam(Parameter("nnc_pretrain",yesno[0],yesno,"Enable (yes) or disable (no) the pre - training process."));

    addParam(Parameter("nnc_balanceclass",yesno[0],yesno,"Enable or disabled balanced classes during train (yes|no)"));
    addParam(Parameter("nnc_outputfile","","The output file for nnc"));
    use_balancedclass=false;
}

double  NNCModel::getTrainError()
{
    if(use_balancedclass==false) return Model::getTrainError();
    else return getClassErrorPerClass(trainDataset);
}

double  NNCModel::getDerivative(Data &x,int pos)
{
    return program->EvalDeriv(x.data(),pos);
}
double  NNCModel::getDerivative2(Data &x,int pos)
{
    return program->EvalDeriv2(x.data(),pos);
}

double  NNCModel::getOutput(Data &x)
{

    return program->Eval(x.data());
}

double  NNCModel::getOutput(double *x)
{
    return program->Eval(x);
}

const int delta = 100;
double  NNCModel::preTrain(vector<int> &result)
{
    MlpProblem *tProblem = new MlpProblem();
    tProblem->disableRemoveData();
    tProblem->setTrainSet(trainDataset);

    Data w;
    const int nodes = 5;
    w.resize( nodes * (trainDataset->dimension()+2));
    for(int i=0;i<(int)w.size();i++)
         w[i]=(2.0*rand()*1.0/RAND_MAX-1.0);

        tProblem->funmin(w);
  //  tProblem->setWeights(w);
    tProblem->setDimension(w.size());
    Data xu,xl;
    xu.resize(w.size());
    xl.resize(w.size());
    for(int j=0;j<(int)w.size();j++)
    {
        xl[j]=-10.0;
        xu[j]= 10.0;

    }
    tProblem->setLeftMargin(xl);
    tProblem->setRightMargin(xu);

    Optimizer *method=NULL;

    method = new Genetic();
    method->setParam("gen_maxiters","200");
    method->setParam("gen_count","200");
    method->setParam("gen_lrate","0.005");
    method->setParam("opt_localsearch","crossover");
    method->setParam("opt_debug","yes");
    method->setParam("opt_termination","maxiters");

    tProblem->setOptimizer(method);

    tProblem->trainModel();
    w=tProblem->getBestx();

    double yy=tProblem->funmin(w);

    Bfgs *tmethod = new Bfgs();
    tProblem->disableBound();
    tmethod->setProblem(tProblem);
    tmethod->setPoint(w,yy);
    tmethod->solve();
     yy=tProblem->funmin(w);
     tmethod->getPoint(w,yy);
    delete tmethod;
    printf("Final pre training = %20.10lg\n",yy);

    Converter con(w,w.size()/(trainDataset->dimension()+2),trainDataset->dimension());
    con.convert(result);

    for(int i=0;i<(int)result.size();i++)
    {
        result[i]+=delta;
    }
    delete method;
    delete tProblem;
    return yy;
}


void    NNCModel::makeProgram(int dimension)
{
    if(program!=NULL) delete program;
    program = new NncProgram(dimension,this);
    program->makeGrammar();
}


int    NNCModel::setChromosomeInParser(vector<int> &genome)
{
    return program->setChromosomeInParser(genome);
}

void  NNCModel::trainModel()
{
        loadTrainSet();
        loadTestSet();
        use_balancedclass=getParam( "nnc_balanceclass").getValue()=="yes";
        if(trainDataset == NULL) return;
        if(pop!=NULL) delete pop;
        makeProgram(trainDataset->dimension());

        vector<int> ruleResult;
        bool preTrainFlag = getParam("nnc_pretrain").getValue()=="yes";
        if(preTrainFlag)
        {
            double yy = preTrain(ruleResult);
            pop = new Population(getParam("nnc_popcount").getValue().toInt(),
                             (int)ruleResult.size(),
                             program,getModelSeed());
	    for(int i=0;i<(int)ruleResult.size();i++)
	    {
            ruleResult[i]-=delta;
	    }
        pop->setChromosome(100,ruleResult,yy);
        }
        else
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
    QString hasSteadyState = getParam("nnc_steadystate").getValue();
    if(hasSteadyState=="yes")
	    pop->enableSteadyState();
    else
	    pop->disableSteadyState();
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

    /**
     */
    pop->setLocalSearchGenerations(LI);
    pop->setLocalSearchItems(LC);
    for(int g=1;g<=gens;g++)
    {
        pop->nextGeneration();
        if(g%50==0)
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
    setParam("nnc_lsearchmethod","bfgs");
    localSearchItem(0);
    setParam("nnc_lsearchmethod",Lmethod);
    pop->evaluateBestFitness();

    QString output=getParam("nnc_outputfile").getValue();
    if(!output.isEmpty() && testDataset!=NULL)
    {
        QFile fp(output);
        if(fp.open(QIODevice::WriteOnly))
        {
            QTextStream st(&fp);
            int count = testDataset->count();
            for(int i=0;i<count;i++)
            {
                double realValue = testDataset->getYPoint(i);
                double estValue  = getOutput(testDataset->getXPoint(i).data());
                double estClaSS  = testDataset->getClass(estValue);
                st<<realValue<<"  "<<estValue<<" "<<estClaSS<<"\n";
            }
            fp.close();
        }
    }
}

void        NNCModel::localSearchItem(int pos)
{
    Data w;
    vector<int> xx;
    double yy;
    int redo = 0;
    double wf = getParam("nnc_weightfactor").getValue().toDouble();
    QString Lmethod = getParam("nnc_lsearchmethod").getValue();

    if(Lmethod == "none" || Lmethod=="siman" || Lmethod=="mutate"
            ||Lmethod=="crossover") return;
       if(Lmethod == "random")
       {
        pop->localSearch(pos);
        return;
       }

        pop->getChromosome(pos,xx,yy);
        vector<int> oldx = xx;
        if(fabs(yy)>1e+10) return;
        string st = program->printRandomProgram(xx,redo);
        if(!program->Parse(st)) return;
        parser->makeVector(st);
        string pt = parser->print();

        parser->getWeights(w);
        trialProblem->resetFunctionCalls();
        trialProblem->funmin(w);
    //    trialProblem->setWeights(w);
        trialProblem->setDimension(w.size());
        Data xu,xl;
        xu.resize(w.size());
        xl.resize(w.size());
        for(int j=0;j<w.size();j++)
        {
            xl[j]=-wf*fabs(w[j]);
            xu[j]= wf*fabs(w[j]);

        }
        trialProblem->setLeftMargin(xl);
        trialProblem->setRightMargin(xu);

        if(getParam("nnc_enablebound").getValue()=="yes")
            trialProblem->enableBound();
        else
            trialProblem->disableBound();
    	if(use_balancedclass) 
	{
		trialProblem->setParam("mlp_balanceclass","yes");
	}

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
        if(Lmethod == "mbfgs")
            method = new MBfgs;
        else
        {
            method = new Genetic();
            method->setParam("gen_maxiters","50");
            method->setParam("gen_lrate","0.00");
            method->setParam("opt_localsearch","none");
        }
        method->setParam("opt_debug","no");

        trialProblem->setOptimizer(method);

        if(Lmethod == "bfgs")
            ((Bfgs *)method)->setPoint(w,yy);
        else
        if(Lmethod == "lbfgs")
            ((Lbfgs *)method)->setPoint(w,yy);
        else
        if(Lmethod=="adam")
            ((Adam *)method)->setPoint(w,yy);
        else
        if(Lmethod=="mbfgs")
            ((MBfgs *)method)->setPoint(w,yy);
        else
            ((Genetic *)method)->setBest(w,yy);
        trialProblem->trainModel();
        w=trialProblem->getBestx();
	yy=trialProblem->funmin(w);
	if(Lmethod == "genetic")
	{
		Bfgs *tmethod = new Bfgs();
            	trialProblem->disableBound();
		tmethod->setProblem(trialProblem);
		tmethod->setPoint(w,yy);
        tmethod->solve();
	}
        w=trialProblem->getBestx();
        yy=trialProblem->funmin(w);
        delete method;
        Converter con(w,w.size()/(trainDataset->dimension()+2),trainDataset->dimension());
        con.convert(xx);
        redo = 0;
        if(xx.size()>pop->getSize())
        {
            printf("Problem in sizes: %ld -> %d \n",xx.size(),pop->getSize());
            return;
        }
        string ss = program->printRandomProgram(xx,redo);
        if(!program->Parse(ss))
        {
            printf("Local search problme in parsing %s \n",ss.c_str());
            return ;
        }
        double newy = pop->fitness(xx);
        if(fabs(newy)>fabs(yy)) return;
        printf("Local Search[%lf]->%lf\n",yy,newy);
        pop->setChromosome(pos,xx,newy);
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
