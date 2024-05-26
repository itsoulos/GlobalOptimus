#include "gwooptimizer.h"
GWOoptimizer::GWOoptimizer()
{
    addParam(Parameter("gwo_agents", "100", "Number of gwo agents"));
    addParam(Parameter("gwo_maxiters", "200", "Number of gwo max iters"));
    addParam(Parameter("gwo_termination", "similarity", "Termination method for gwo"));
}

void    GWOoptimizer::init()
{
    terminationMethod = getParam("gwo_termination").getValue();
    SearchAgents_no = getParam("gwo_agents").getValue().toInt();
    Max_iter = getParam("gwo_maxiters").getValue().toInt();
    Alpha_pos.resize(myProblem->getDimension());
    Beta_pos.resize(myProblem->getDimension());
    Delta_pos.resize(myProblem->getDimension());
    sampleFromProblem(SearchAgents_no,Positions,fitnessArray);
    iter = 0;
    int p1=0,p2,p3;
    double best=fitnessArray[0];
    for(int i=0;i<fitnessArray.size();i++)
    {
        if(fitnessArray[i]<best)
        {
            best= fitnessArray[i];
            p1 = i;
        }
    }
    p2 = 0;
    if(p2==p1) p2 = 1;
    best = fitnessArray[p2];
    for(int i=0;i<fitnessArray.size();i++)
    {
        if(fitnessArray[i]<best && i!=p1)
        {
            best = fitnessArray[i];
            p2 = i;
        }
    }
    p3 = 0;
    if(p3==p1) p3++;
    if(p3==p2) p3++;
    best = fitnessArray[p3];
    for(int i=0;i<fitnessArray.size();i++)
    {
        if(fitnessArray[i]<best && i!=p1 && i!=p2 )
        {
            best = fitnessArray[i];
            p3 =i;
        }
    }
    Alpha_pos = Positions[p1];
    Alpha_score = fitnessArray[p1];
    Beta_pos = Positions[p2];
    Beta_score = fitnessArray[p2];
    Delta_pos = Positions[p3];
    Delta_score = fitnessArray[p3];
}

void    GWOoptimizer::step()
{
    for(int i=0;i<SearchAgents_no;i++)
    {
        double fitness = myProblem->statFunmin(Positions[i]);
        if(fitness<Alpha_score)
        {
            Alpha_score=fitness;
            Alpha_pos=Positions[i];
        }


        if(fitness>Alpha_score && fitness<Beta_score)
        {
            Beta_score=fitness; //Update Beta
            Beta_pos=Positions[i];
        }
        if(fitness>Alpha_score && fitness>Beta_score && fitness<Delta_score)
        {
            Delta_score=fitness; //Update Delta
            Delta_pos=Positions[i];
        }
    }
    int dim = myProblem->getDimension();
    double a=2-1.0*((2.0)/Max_iter); //'a' decreases linearly from 2 to 0
    //Update the position of search agents including omegas
    for (int i=0;i<SearchAgents_no;i++)
    {
        //			cout<<" Inside for loop 3 \n";

        for (int j=0;j<dim;j++)
        {
            //	cout<<" Inside for loop 3.1 \n";
            double r1=drand48();  //r1 is random number in [0,1]
            double r2=drand48();  //r1 is random number in [0,1]
            double A1=2*a*r1-a; //Equation (3.3)
            double C1=2*r2; //Equation (3.4)
            double D_alpha=fabs(C1*Alpha_pos[j]-Positions[i][j]);//Equation (3.5)-part 1
            double X1=Alpha_pos[j]-A1*D_alpha; //Equation (3.6)-part 1

            r1=drand48();
            r2=drand48();
            double A2=2*a*r1-a; //Equation (3.3)
            double C2=2*r2; //Equation (3.4)
            double D_beta=fabs(C2*Beta_pos[j]-Positions[i][j]); //Equation (3.5)-part 2
            double X2=Beta_pos[j]-A2*D_beta; //Equation (3.6)-part 2

            r1=drand48();
            r2=drand48();
            double A3=2*a*r1-a; //Equation (3.3)
            double C3=2*r2; //Equation (3.4)
            double D_delta=fabs(C3*Delta_pos[j]-Positions[i][j]); //Equation (3.5)-part 3
            double X3=Delta_pos[j]-A3*D_delta; //Equation (3.6)-part 3

            Positions[i][j]=(X1+X2+X3)/3; //Equation (3.7)
        }
    }

    iter= iter+1;
}


bool    GWOoptimizer::terminated()
{
    double besty;
    besty = Alpha_score;
    //chromosomes.getBestWorstValues(besty,worsty);
    if(iter>=Max_iter) return true;
    if(terminationMethod=="doublebox")
        return doubleBox.terminate(besty);
    else
        if(terminationMethod=="similarity")
            return similarity.terminate(besty);
    return false;
}

void    GWOoptimizer::showDebug()
{
    printf("Iter=%4d BEST VALUE=%10.4lf\n",
           iter,Alpha_score);
}

void    GWOoptimizer::done()
{
    Alpha_score = localSearch(Alpha_pos);
}

GWOoptimizer::~GWOoptimizer()
{

}