#include "ParallelBGwo.h"

ParallelBGwo::ParallelBGwo()
{
    before = std::chrono::system_clock::now();
    addParam(Parameter("prop", 0, 0, 1, "0 or 1"));
    addParam(Parameter("population", "100", "The number of elements."));
    addParam(Parameter("maxiters", 100, 10, 10000, "Maximum iters"));
    addParam(Parameter("localsearchRate", 0.01, 0.00, 1.0, "Localsearch rate"));
    addParam(Parameter("subPopulation", "10", "The number of subelements."));
    addParam(Parameter("propagationNumber", 0, 0, 50, "Number of positions to propagate."));
    getParam("subPopEnable").getValue().trimmed();
    QStringList propagationMethodList;
    propagationMethodList << "1to1" << "1toN" << "Nto1" << "NtoN";
    addParam(Parameter("propagationMethod", propagationMethodList[0],
                       propagationMethodList,
                       "Propagation strategy: 1to1, 1toN, Nto1, NtoN"));
    addParam(Parameter("subPopEnable", "1", "Number of subpopulations required to terminate."));
    addParam(Parameter("propagationRate", "10", "Number of positions to propagate."));
    addParam(Parameter("k", 2.2, 0.1, 10.0, "BGWO nonlinear convergence coefficient"));
}

void ParallelBGwo::init()
{
    prop = getParam("prop").getValue().toInt();
    subPopulation = getParam("subPopulation").getValue().toInt();
    dimension = myProblem->getDimension();
    population = getParam("population").getValue().toInt();
    maxIterations = getParam("maxiters").getValue().toInt();
    localSearchRate = getParam("localsearchRate").getValue().toDouble();
    propagationNumber = getParam("propagationNumber").getValue().toInt();
    propagationMethod = getParam("propagationMethod").getValue();
    subPopEnable = getParam("subPopEnable").getValue().toInt();
    propagationRate = getParam("propagationRate").getValue().toInt();
    subPopEnableStr = getParam("subPopEnable").getValue().trimmed();
    k_coeff = getParam("k").getValue().toDouble();

    printf("prop=%d\tpropagationNumber=%d\tpropagationRate=%d\tpropagationMethod=%s\n",
           prop, propagationNumber, propagationRate,
           propagationMethod.toStdString().c_str());
    terminationMethod = getParam("terminationVote").getValue();
    iteration = 0;
    bestF2x.resize(subPopulation);
    bestF2xOld.resize(subPopulation);
    bSamply.resize(dimension);
    similarityCurrentCountBest.resize(subPopulation);
    if (subPopEnableStr.compare("ANY", Qt::CaseInsensitive) == 0) {
           subPopEnable = 1;
       }
       else if (subPopEnableStr.compare("MAJORITY", Qt::CaseInsensitive) == 0) {
           subPopEnable = subPopulation / 2;
       }
       else if (subPopEnableStr.compare("ALL", Qt::CaseInsensitive) == 0) {
           subPopEnable = subPopulation;
       }
       else {
           bool ok = false;
           int value = subPopEnableStr.toInt(&ok);

           if (ok)
               subPopEnable = value;
           else
               subPopEnable = 1;
       }
    for (int k = 0; k < subPopulation; k++)
    {
        bestF2x.at(k) = 1e+100;
        bestF2xOld.at(k) = 1e+100;
        similarityCurrentCountBest.at(k) = 0;
    }

    bF2x = 1e+100;
    bestF2x2.resize(subPopulation);
    bestF2x3.resize(subPopulation);
    bestSamply.resize(subPopulation, vector<double>(dimension));
    bestSamply2.resize(subPopulation, vector<double>(dimension));
    bestSamply3.resize(subPopulation, vector<double>(dimension));
    sub_size = population / subPopulation;

    for (int k = 0; k < subPopulation; k++)
    {
        bestF2x2[k] = 1e+100;
        bestF2x3[k] = 1e+100;
        bestSamply[k].assign(dimension, 0.0);
        bestSamply2[k].assign(dimension, 0.0);
        bestSamply3[k].assign(dimension, 0.0);
    }

    sampleFromProblem(population, samples, fitnessArray);


    for (int k = 0; k < subPopulation; k++)
    {
        updateLeadersForSubpop(k);

        if (bestF2x[k] < bF2x)
        {
            bF2x = bestF2x[k];
            bSamply = bestSamply[k];
        }
    }
}


double ParallelBGwo::randomDouble(double min, double max)
{
    return min + (max - min) * (rand() / (double)RAND_MAX);
}

// Nonlinear convergence factor a(t)
double ParallelBGwo::convergenceFactorA() const
{
    const double T = (double)maxIterations;
    const double t = (double)(iteration + 1);
    double ratio = (t - 1.0) / T;

    const double eps = 1e-12;
    ratio = std::min(1.0 - eps, std::max(eps, ratio));

    const double denom = std::log(ratio);
    const double exponent = k_coeff / denom;   //
    return 2.0 * std::exp(exponent);
}

// Update alpha/beta/delta within subpopulation
void ParallelBGwo::updateLeadersForSubpop(int k)
{
    int start = k * sub_size;
    int end = std::min(start + sub_size, population);

    a1 = 1e100, a2 = 1e100, a3 = 1e100;
    std::vector<double> x1(dimension), x2(dimension), x3(dimension);

    for (int i = start; i < end; ++i)
    {
        const double fi = fitnessArray[i];

        if (fi < a1)
        {
            a3 = a2; x3 = x2;
            a2 = a1; x2 = x1;
            a1 = fi; x1 = samples[i];
        }
        else if (fi < a2)
        {
            a3 = a2; x3 = x2;
            a2 = fi; x2 = samples[i];
        }
        else if (fi < a3)
        {
            a3 = fi; x3 = samples[i];
        }
    }
    if(end<=start)
        return;

    bestF2x[k]  = a1;
    bestSamply[k]  = x1;
    if(a2 >= 1e99)
        {
         bestF2x2[k] = a1;
         bestSamply2[k] = x1;
        }
    else
    {
     bestF2x2[k] = a2;
     bestSamply2[k] = x2;
    }
    if(a3 >= 1e99)
        {
         bestF2x3[k] = a3;
         bestSamply3[k] = x3;
        }
}



void ParallelBGwo::step()
{
    iteration++;
    const double a = convergenceFactorA();
    double minValue;
    int pos = 0;

#pragma omp parallel for num_threads(subPopulation)
    for (int k = 0; k < subPopulation; k++)
    {
        int start = k * sub_size;
        int end = min(start + sub_size, population);

        // take current leaders snapshot for this subpop
        std::vector<double> Alpha = bestSamply[k];
        std::vector<double> Beta  = bestSamply2[k];
        std::vector<double> Delta = bestSamply3[k];

        for (int i = start; i < end; i++)
        {
            std::vector<double> newPosition(dimension);

            // BGWO stochastic factors: R in [0.5, 1.5]
            const double R1 = randomDouble(0.5, 1.5);
            const double R2 = randomDouble(0.5, 1.5);
            const double R3 = randomDouble(0.5, 1.5);

            // C = 2r
            const double Ca = 2.0 * randomDouble(0.0, 1.0);
            const double Cb = 2.0 * randomDouble(0.0, 1.0);
            const double Cc = 2.0 * randomDouble(0.0, 1.0);

            // A = a(2r-1), with nonlinear a
            const double Aa = a * (2.0 * randomDouble(0.0, 1.0) - 1.0);
            const double Ab = a * (2.0 * randomDouble(0.0, 1.0) - 1.0);
            const double Ac = a * (2.0 * randomDouble(0.0, 1.0) - 1.0);

            for (int j = 0; j < dimension; j++)
            {
                const double Xi = samples[i][j];

                const double Da = std::fabs(Ca * Alpha[j] - Xi);
                const double Db = std::fabs(Cb * Beta[j]  - Xi);
                const double Dc = std::fabs(Cc * Delta[j] - Xi);

                const double Xa = R1 * (Alpha[j] - Aa * Da);
                const double Xb = R2 * (Beta[j]  - Ab * Db);
                const double Xc = R3 * (Delta[j] - Ac * Dc);

                newPosition[j] = (Xa + Xb + Xc) / 3.0;
            }


#pragma omp critical
            {
                minValue = myProblem->statFunmin(newPosition);

                if (minValue < fitnessArray.at(i))
                {
                    samples.at(i) = newPosition;
                    fitnessArray.at(i) = minValue;
                }

                if (minValue < bF2x)
                {
                    bF2x = minValue;
                    bSamply = newPosition;
                    pos = i;
                }
            }
        }

        // update leaders for this subpop after moves
        {
            updateLeadersForSubpop(k);
        }

        if (prop && iteration % propagationRate == 0 && subPopulation > 1)
            propagate();
    }

    for (int i = 0; i < (int)samples.size(); i++)
    {
        if (localSearchRate > 0.0)
        {
            double r = rand() * 1.0 / RAND_MAX;
            if (r < localSearchRate)
            {
                fitnessArray[i] = localSearch(samples[i]);
            }
        }
    }
}


bool ParallelBGwo::checkCluster(int subCluster)
{
    double difference = fabs((bestF2xOld.at(subCluster)) - bestF2x.at(subCluster));
    bestF2xOld.at(subCluster) = bestF2x.at(subCluster);
    if (difference < 1e-6)
        similarityCurrentCountBest.at(subCluster)++;
    else
        similarityCurrentCountBest.at(subCluster) = 0;
    if (similarityCurrentCountBest.at(subCluster) > 5)
        return true;
    return false;
}


void ParallelBGwo::propagate()
{
    if (propagationMethod == "1to1")
    {
        int sender = rand() % subPopulation;
        int receiver;
        do
        {
            receiver = rand() % subPopulation;
        } while (receiver == sender);

        propagateBetween(sender, receiver);
    }
    else if (propagationMethod == "1toN")
    {
        int sender = rand() % subPopulation;
        for (int receiver = 0; receiver < subPopulation; ++receiver)
        {
            if (receiver != sender)
                propagateBetween(sender, receiver);
        }
    }
    else if (propagationMethod == "Nto1")
    {
        int receiver = rand() % subPopulation;
        for (int sender = 0; sender < subPopulation; ++sender)
        {
            if (sender != receiver)
                propagateBetween(sender, receiver);
        }
    }
    else if (propagationMethod == "NtoN")
    {
        for (int sender = 0; sender < subPopulation; ++sender)
        {
            for (int receiver = 0; receiver < subPopulation; ++receiver)
            {
                if (sender != receiver)
                    propagateBetween(sender, receiver);
            }
        }
    }
}

void ParallelBGwo::propagateBetween(int sender, int receiver)
{
    int senderStart = sender * sub_size;
    int senderEnd = std::min(senderStart + sub_size, population);
    int receiverStart = receiver * sub_size;
    int receiverEnd = std::min(receiverStart + sub_size, population);

    std::vector<std::pair<double, int>> senderFitnessIndex;
    for (int i = senderStart; i < senderEnd; ++i)
    {
        senderFitnessIndex.emplace_back(fitnessArray[i], i);
    }
    std::sort(senderFitnessIndex.begin(), senderFitnessIndex.end());

    std::vector<std::pair<double, int>> receiverFitnessIndex;
    for (int i = receiverStart; i < receiverEnd; ++i)
    {
        receiverFitnessIndex.emplace_back(fitnessArray[i], i);
    }
    std::sort(receiverFitnessIndex.begin(), receiverFitnessIndex.end(), std::greater<>());

    for (int i = 0; i < propagationNumber; ++i)
    {
        if (i >= (int)senderFitnessIndex.size() || i >= (int)receiverFitnessIndex.size())
            break;

        int bestIndex = senderFitnessIndex[i].second;
        int worstIndex = receiverFitnessIndex[i].second;

        samples[worstIndex] = samples[bestIndex];
        fitnessArray[worstIndex] = fitnessArray[bestIndex];
    }
}
bool ParallelBGwo::terminated()
{
    int terminatedCount = 0;
    const double bestValue=bF2x;
    if(terminationMethod=="doublebox")
            return doubleBox.terminate(bestValue);

        else if(terminationMethod=="similarity")
                return similarity.terminate(bestValue);
        else  {
        for (int k = 0; k < subPopulation; k++)
            if (checkCluster(k))
                terminatedCount++;
           return (terminatedCount >= subPopEnable);
              }
}



void ParallelBGwo::showDebug() {}

void ParallelBGwo::done()
{
    bF2x = localSearch(bSamply);
    after = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
    auto ms = milliseconds.count();
    std::cout << "Douration: " << (double)ms / 1000.0 << " sec" << std::endl;
}


ParallelBGwo::~ParallelBGwo() {}

