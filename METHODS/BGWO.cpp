#include "BGWO.h"
BGWO::BGWO()
{
    addParam(Parameter("bgwo_agents", 100,10,1000, "Number of gwo agents"));
    addParam(Parameter("bgwo_maxiters", 100,10,1000, "Number of gwo max iters"));
    addParam(Parameter("lrate",0.0,0.0,1.0,"The local search rate"));
    addParam(Parameter("bgwo_k", 2.2,2.0,3.0, " k"));


}

void    BGWO::init()
{
    SearchAgents_no = getParam("bgwo_agents").getValue().toInt();
    Max_iter = getParam("bgwo_maxiters").getValue().toInt();
    k = getParam("bgwo_k").getValue().toDouble(); // paper: k = 2.2
    Alpha_pos.resize(myProblem->getDimension());
    Beta_pos.resize(myProblem->getDimension());
    Delta_pos.resize(myProblem->getDimension());
    sampleFromProblem(SearchAgents_no,Positions,fitnessArray);
    localSearchRate=getParam("lrate").getValue().toDouble();
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
    sumMean = accumulate(fitnessArray.begin(), fitnessArray.end(), 0.0);
}

void BGWO::step()
{
    const int dim = myProblem->getDimension();

    // (A) Αξιολόγηση πληθυσμού + ενημέρωση Alpha/Beta/Delta

    for (int i = 0; i < SearchAgents_no; ++i)
    {
        const double fit = myProblem->statFunmin(Positions[i]);
        fitnessArray[i] = fit;

        // Update Alpha / Beta / Delta (καλύτερες 3 λύσεις)
        if (fit < Alpha_score)
        {
            Alpha_score = fit;
            Alpha_pos   = Positions[i];
        }
        else if (fit < Beta_score)
        {
            Beta_score = fit;
            Beta_pos   = Positions[i];
        }
        else if (fit < Delta_score)
        {
            Delta_score = fit;
            Delta_pos   = Positions[i];
        }
    }

    sumMean = std::accumulate(fitnessArray.begin(), fitnessArray.end(), 0.0);

    // (B) BGWO: Μη-γραμμικός convergence factor a(t)
    //     Paper Eq. (8):
    //       a = 2 * e^( k / ln( (t-1)/T ) )

    const double T = static_cast<double>(Max_iter);
    const double t = static_cast<double>(iter) + 2.0;

    double ratio = (t - 1.0) / T;                 // (t-1)/T
    const double eps = 1e-12;
    ratio = std::clamp(ratio, eps, 1.0 - eps);    // κρατάμε ratio ∈ (0,1) για να ορίζεται ln(ratio)

    const double a = 2.0 * std::exp(k / std::log(ratio));

    // Random helpers
    auto rand01 = []() -> double { return rand() * (1.0 / (double)RAND_MAX); };

    // BGWO  απαιτεί R1,R2,R3 ∈ [0.5, 1.5]
    auto randR = [&]() -> double { return 0.5 + rand01(); };  // [0.5,1.5]

    // (C) Update θέσεων (BGWO stochastic position update)
    // BGWO stochastic update (Paper Eq. (7)):
    //   Xa = R1 * (Xα - Aα*Dα)
    //   Xb = R2 * (Xβ - Aβ*Dβ)
    //   Xc = R3 * (Xδ - Aδ*Dδ)
    //   X_new = (Xa + Xb + Xc) / 3

    for (int i = 0; i < SearchAgents_no; ++i)
    {
        for (int j = 0; j < dim; ++j)
        {
            // ---- Alpha component
            double r1 = rand01();
            double r2 = rand01();
            double A1 = 2.0 * a * r1 - a;          //  A = a*(2r-1)
            double C1 = 2.0 * r2;                  //  C = 2r
            double D_alpha = std::fabs(C1 * Alpha_pos[j] - Positions[i][j]);
            double base1 = Alpha_pos[j] - A1 * D_alpha;

            // ---- Beta component
            r1 = rand01();
            r2 = rand01();
            double A2 = 2.0 * a * r1 - a;
            double C2 = 2.0 * r2;
            double D_beta = std::fabs(C2 * Beta_pos[j] - Positions[i][j]);
            double base2 = Beta_pos[j] - A2 * D_beta;

            // ---- Delta component
            r1 = rand01();
            r2 = rand01();
            double A3 = 2.0 * a * r1 - a;
            double C3 = 2.0 * r2;
            double D_delta = std::fabs(C3 * Delta_pos[j] - Positions[i][j]);
            double base3 = Delta_pos[j] - A3 * D_delta;

            // BGWO stochastic scaling (Paper Eq. (7))
            const double R1 = randR();
            const double R2 = randR();
            const double R3 = randR();

            const double Xa = R1 * base1;
            const double Xb = R2 * base2;
            const double Xc = R3 * base3;

            double xnew = (Xa + Xb + Xc) / 3.0;    // Paper Eq. (7)

            Positions[i][j] = xnew;
        }
    }

    iter += 1;
}


bool    BGWO::terminated()
{
    double besty;
    besty = Alpha_score;
    //chromosomes.getBestWorstValues(besty,worsty);
    if(iter>=Max_iter) return true;

    bool t1=false,t2=false,t3=false;
    if(terminationMethod=="similarity" || terminationMethod=="all")
    {
        similarity.setSimilarityIterations(12);
    t1=similarity.terminate(besty);
    if(t1) return true;
    }
    if(terminationMethod=="mean" || terminationMethod=="all")
    {
        mean.setMeanIterations(12);
        t2=mean.terminate(sumMean);
    if(t2) return true;
    }
    if(terminationMethod=="doublebox" || terminationMethod=="all")
    {
        t3=doubleBox.terminate(besty);
    if(t3) return true;
    }
    if(terminationMethod=="all") return (t1 || t2|| t3);
    return false;

/*
    if(terminationMethod=="doublebox")
    {
        return doubleBox.terminate(besty);
    }
    if(terminationMethod=="similarity")
    {
        similarity.setSimilarityIterations(12);
        return similarity.terminate(besty);
    }
    if(terminationMethod=="mean")
    {
        mean.setMeanIterations(12);
        return mean.terminate(sumMean);
    }*/

    return false;
}

void    BGWO::showDebug()
{
    methodLogger->printMessage(
        QString::asprintf("Iter=%4d BEST VALUE=%10.4lf",iter,Alpha_score));
    methodLogger->printMessage(QString::asprintf("Iter=%4d BEST VALUE=%10.4lf",iter,sumMean));
}

void   BGWO::done()
{
    Alpha_score = localSearch(Alpha_pos);
}

BGWO::~BGWO()
{

}
