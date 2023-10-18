#include <METHODS/neldermead.h>
NelderMead::NelderMead()
{
    addParam(Parameter("nm_population","100","Number of items in population"));
    addParam(Parameter("nm_alpha","1.0","Alpha value of the algorithm"));
    addParam(Parameter("nm_gamma","2.0","Gamma parameter of the algorithm"));
    addParam(Parameter("nm_rho","0.5","Rho parameter of the algorithm"));
    addParam(Parameter("nm_sigma","0.5","Sigma parameter of the algorithm"));
    addParam(Parameter("nm_maxiters","100","Maximum number of iterations"));
    haveInitialized = false;
}

void    NelderMead::order()
{
    sort(population.begin(), population.end());//[](auto a,auto b){return a.first < b.first;});
}

void    NelderMead::center()
{
    centerPoint.resize(population_size);
    for (int i = 0; i < population_size; i++)
    {
        centerPoint[i] = 0.0;
    }
    for (int i = 0; i < population_count-1; i++)
    {
        for (int j = 0; j < population_size; j++)
        {
            centerPoint[j] += population[i].second[j];
        }
    }
    for (int i = 0; i < population_size; i++)
    {
        centerPoint[i] /= population_count-1;
        if (centerPoint[i] < lmargin[i])
            centerPoint[i] = lmargin[i];
        if (centerPoint[i] > rmargin[i])
            centerPoint[i] = rmargin[i];
    }
}

void    NelderMead::reflection()
{
    reflectedPoint.resize(population_size);
    for (int j = 0; j < population_size; j++)
    {
        reflectedPoint[j] = centerPoint[j] + alpha * (centerPoint[j] - population[population_count - 1].second[j]);
    }
    yreflectedPoint = myProblem->statFunmin(reflectedPoint);
}

void    NelderMead::expansion()
{
    expandedPoint.resize(population_size);
    for (int j = 0; j < population_size; j++)
    {
        expandedPoint[j] = centerPoint[j] + gamma * (reflectedPoint[j] - centerPoint[j]);
    }
    yexpandedPoint = myProblem->funmin(expandedPoint);
}

void    NelderMead::contraction()
{
    contractedPoint.resize(population_size);
    for (int j = 0; j < population_size; j++)
    {
        contractedPoint[j] = centerPoint[j] + ro * (reflectedPoint[j] - centerPoint[j]);
    }
    ycontractedPoint = myProblem->statFunmin(contractedPoint);
}

void    NelderMead::contractionB()
{
    for (int j = 0; j < population_size; j++)
    {
        contractedPoint[j] = centerPoint[j] + ro * (population[population_count - 1].second[j] - centerPoint[j]);
    }
    ycontractedPoint = myProblem->statFunmin(contractedPoint);
}

void    NelderMead::shrink()
{
    for (int i = 1; i < population_count; i++)
    {
        for (int j = 0; j < population_size; j++)
        {
            population[i].second[j] = population[0].second[j] + sigma * (population[i].second[j] - population[0].second[j]);
            population[i].first = myProblem->statFunmin(population[i].second);
        }
    }
}

void    NelderMead::setPoint(Data &x,double &y)
{
    haveInitialized = true;
    bestPoint.resize(x.size());
    bestPoint = x;
    ybestPoint=y;
}

void    NelderMead::getPoint(Data &x,double &y)
{
    double dmin=1e+100;
    int imin = -1;
    for(int i=0;i<population_count;i++)
    {
        if(population[i].first<dmin)
        {
            dmin = population[i].first;
            imin =i;
        }
    }
    bestPoint = population[imin].second;
    x= bestPoint;
    y = ybestPoint;
}

void    NelderMead::init()
{
    population_count = getParam("nm_population").getValue().toInt();
    alpha= getParam("nm_alpha").getValue().toDouble();
    gamma= getParam("nm_gamma").getValue().toDouble();
    ro =   getParam("nm_rho").getValue().toDouble();
    sigma= getParam("nm_sigma").getValue().toDouble();
    max_generations=getParam("nm_maxiters").getValue().toInt();

    generation = 0;
    assert(population_count >= 4);
    population_size = myProblem->getDimension();
    population.resize(population_count);
    for (int i = 0; i < population_count; i++)
    {
        population[i].second.resize(population_size);
    }
    lmargin = myProblem->getLeftMargin();
    rmargin = myProblem->getRightMargin();
    centerPoint.resize(population_size);
    reflectedPoint.resize(population_size);
    expandedPoint.resize(population_size);
    contractedPoint.resize(population_size);

    if(!haveInitialized)
    {
        bestPoint = myProblem->getSample();
        ybestPoint = myProblem->statFunmin(bestPoint);
    }
    haveInitialized =false;
    for (int i = 0; i < population_count; i++)
    {
        if(i==0)
        {
            population[i].second = bestPoint;
            population[i].first = ybestPoint;
        }
        else
        {
            population[i].second = myProblem->getSample();
            population[i].first = myProblem->statFunmin(population[i].second);
        }
    }

    sum = accumulate(&population.begin()->first, &population.end()->first, 0);
    // if (sum == 0)
    //     assert(sum > 0);
    sum = sum / population_count;
    n = 0;
}

void    NelderMead::step()
{

start:
    order();
    center();
    ybestPoint = population.begin()->first;
    worst = (population.end())->first;
    secondWorst = population[population_count - 2].first;
    reflection();
    if (yreflectedPoint < secondWorst && yreflectedPoint > ybestPoint)
    {
        population[population_count - 1].second = reflectedPoint;
        population[population_count - 1].first = yreflectedPoint;
        goto start;
    }
    else if (yreflectedPoint < ybestPoint)
    {
        expansion();
        if (yexpandedPoint < yreflectedPoint)
        {
            population[population_count - 1].second = expandedPoint;
            population[population_count - 1].first = yexpandedPoint;
            goto start;
        }
        else
        {
            population[population_count - 1].second = reflectedPoint;
            population[population_count - 1].first = yreflectedPoint;
            goto start;
        }
        goto start;
    }
    else
    {
        if (yreflectedPoint < worst)
        {
            contraction();
            if (ycontractedPoint < yreflectedPoint)
            {
                population[population_count - 1].second = contractedPoint;
                population[population_count - 1].first = ycontractedPoint;
                goto start;
            }
            else
                shrink();
        }
        else
        {
            if (ycontractedPoint < worst)
            {
                contractionB();
                population[population_count - 1].second = contractedPoint;
                population[population_count - 1].first = ycontractedPoint;
                goto start;
            }
            else
            {
                shrink();
            }
        }
    }
    newSum = accumulate(&population.begin()->first, &population.end()->first, 0);
    newSum = newSum / population_count;
    ++generation;
}

bool    NelderMead::terminated()
{
    double dd = fabs(newSum - sum);
    if(dd<1e-6 || generation>=10) return true;
    printf("%4d] Generation  change: %10.6lf ybest:%10.6lf \n", generation, dd,ybestPoint);
    if (dd < 1e-8)
        n++;
    else
        n = 0;
    if (n > 15)
        return true;
    sum = newSum;

    return generation >= max_generations;
}

void    NelderMead::showDebug()
{

}

void    NelderMead::done()
{
    ybestPoint = myProblem->statFunmin(population[0].second);

}


NelderMead::~NelderMead()
{

}
