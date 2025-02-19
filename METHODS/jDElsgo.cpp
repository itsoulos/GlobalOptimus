#include "jDElsgo.h"
#include "OPTIMUS/problem.h"

jDElsgo::jDElsgo()
{
    before = std::chrono::system_clock::now();
    addParam(Parameter("de_np", "10n", "The number of agents. Default value 10n"));
    addParam(Parameter("de_f", 0.8, 0.0, 1.0, "DE factor value"));
    addParam(Parameter("de_cr", "0.9", "DE CR parameter"));
    addParam(Parameter("de_tsize", 8, 2, 20, "Tournament size"));
    addParam(Parameter("de_maxiters", 200, 10, 10000, "DE maximum iterations"));
    addParam(Parameter("localsearchRate", 0.05, 0.0, 1.0, "Local search rate"));
    QStringList de_selection;
    de_selection << "random" << "tournament";
    addParam(Parameter("de_selection", de_selection[0], de_selection, "Selection method. Available values: random, tournament"));
}

void jDElsgo::sampleFromProblem(int NP, QVector<Data>& agentx, QVector<double>& agenty)

{
    agentx.clear();
    agenty.clear();
    agentx.resize(NP);
    agenty.resize(NP);

    for (int i = 0; i < NP; ++i) {
        Data individual(myProblem->getDimension());
        for (unsigned long j = 0; j < individual.size(); ++j) {
            individual[j] = myProblem->randomDouble();
        }

        agentx[i] = individual;
        agenty[i] = myProblem->statFunmin(individual);
    }
}

void jDElsgo::init()
{
    localSearchRate = getParam("localsearchRate").getValue().toDouble();
    if(getParam("de_np").getValue()=="10n")
        NP = 10 * myProblem->getDimension();
    else
    NP = getParam("de_np").getValue().toInt();
    F = getParam("de_f").getValue().toDouble();
    CR = getParam("de_cr").getValue().toDouble();
    maxiters = getParam("de_maxiters").getValue().toInt();
    imin = 0;
    iter = 0;
    tsize=getParam("de_tsize").getValue().toInt();
    minindex=-1;
    sampleFromProblem(NP, agentx, agenty);
    if (agentx.empty() || agenty.empty()) {
        return;
    }

    besty = agenty[0];
    bestx = agentx[0];
    for (int i = 1; i < agentx.size(); i++) {
        if (agenty[i] < besty) {
            besty = agenty[i];
            bestx = agentx[i];
            imin = static_cast<int>(i);
        }
    }
    before = std::chrono::system_clock::now();
}

Data jDElsgo::InitRandom()
{
    Data new_individual(myProblem->getDimension());
    if (new_individual.size() != static_cast<std::vector<double>::size_type>(myProblem->getDimension())) {
        return new_individual;
    }
    for (std::vector<double>::size_type j = 0; j < new_individual.size(); j++) {
        new_individual[j] = myProblem->randomDouble();
    }
    return new_individual;
}
void jDElsgo::step()
{
    ++iter;

    for (size_t it = 0; it < MAX_FEs; ++it) {
        int i = it % NP;
        if (i >= static_cast<int>(agentx.size()) || i >= static_cast<int>(agenty.size())) {
            continue;
        }

        if ((it > MAX_FEs / 2) && (i < 2)) {
            agentx[i] = InitRandom();
            lower = 0.01;
            upper = 0.09;
            u = jDEoneIter(imin, agentx);
        } else {
            if (i % 2 == 0) {
                lower = 0.1;
                upper = 0.9;
                u = jDEoneIter(i, agentx);
            } else {
                lower = 0.4;
                upper = 0.6;
                CR = 0.6 + (1.0 - 0.6) * myProblem->randomDouble();
                u = jDEoneIter(i, agentx);
            }
        }

        if (u.empty()) {
            continue;
        }

        ft = myProblem->statFunmin(u);
        if (ft < agenty[i]) {
            agentx[i] = u;
            agenty[i] = ft;

            if (ft < besty) {
                besty = ft;
                bestx = u;
                imin = i;
            }
        }
    }
}

Data jDElsgo::jDEoneIter(int index, QVector<Data>& pop)
{
    if (index >= static_cast<int>(pop.size())) {
        return Data();
    }

    Data trial = pop[index];
    return trial;
}

bool jDElsgo::terminated()
{
        QString term = terminationMethod;
        if(term == "maxiters")
            return iter>=maxiters;
        else
            if(term == "doublebox")
            return doubleBox.terminate(besty);
        else
            if(term == "similarity")
            return similarity.terminate(worsty);
        return false;
}
int    jDElsgo::tournament()
{

    double minvalue=1e+100;
    for(int i=0;i<tsize;i++)
    {
        int pos = rand()  % NP;

        double yy=agenty[pos];
        if(i==0 || yy<minvalue)
        {
            minindex=pos;
            minvalue=yy;
        }
    }
    return minindex;
}
void jDElsgo::showDebug()
{
    bool debug = getParam("opt_debug").getValue() == "yes";
    if (debug) {
        double worsty = *std::max_element(agenty.begin(), agenty.end());
        double sum = std::accumulate(agenty.begin(), agenty.end(), 0.0);
        methodLogger->printMessage(QString::asprintf("DE. Iteration=%5zu BEST=%f WORST=%f SUM=%f", iter, besty, worsty, sum));
    }
}
void jDElsgo::done()
{
    besty = localSearch(bestx);
    showDebug();
    after = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
    auto ms = milliseconds.count();
}

jDElsgo::~jDElsgo()
{

}
