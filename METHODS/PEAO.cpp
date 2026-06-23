#include "PEAO.h"

#include <cstdlib>
#include <limits>

PEAO::PEAO()
{
    before = std::chrono::system_clock::now();

    addParam(Parameter("population", "100", "The number of elements."));
    addParam(Parameter("maxiters", 100, 10, 100000, "Maximum number of iterations."));
    addParam(Parameter("subPopEnable", "1", "Number of subpopulations required to terminate."));
    addParam(Parameter("localsearchRate", 0.01, 0.00, 1.0, "Local search rate."));
    addParam(Parameter("prop", 0, 0, 1, "0 or 1"));
    addParam(Parameter("propagationRate", "10", "Number of positions to propagate."));
    addParam(Parameter("propagationNumber", 0, 0, 50, "Number of positions to propagate."));
    QStringList propagationMethod;
    propagationMethod<<"1to1"<<"1toN"<<"Nto1"<<"NtoN";
    addParam(Parameter("propagationMethod", propagationMethod[0],
                       propagationMethod,
                       "Propagation strategy: 1to1, 1toN, Nto1, NtoN"));
    addParam(Parameter("subPopulation", "10", "Number of subpopulations/groups."));
    addParam(Parameter("communicationThreshold", 10, 1, 1000, "Communication period in iterations."));
    addParam(Parameter("enzymeConcentration", 0.1, 0.0, 1.0, "PEAO enzyme concentration."));
    addParam(Parameter("levyBetaTimes10", 15, 10, 19, "Levy beta multiplied by 10. Default 15 => beta=1.5."));
    addParam(Parameter("communicationStrategyMode", 0, 0, 3,
                       "0=random, 1=optimal replacement, 2=optimal mean replacement, 3=circular optimal replacement"));
}

double PEAO::randomDouble(double minValue, double maxValue)
{
    return minValue + (maxValue - minValue) * (static_cast<double>(rand()) / static_cast<double>(RAND_MAX));
}

double PEAO::uniform01()
{
    return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
}

/*
 * Equation (3):
*/
double PEAO::adaptiveFactor(void) const
{
    const double t = static_cast<double>(iteration);
    const double T = static_cast<double>(maxIterations);

    if (T <= 0.0)
        return 0.0;

    return std::sqrt(t / T);
}

double PEAO::levyBeta(void) const
{
    return static_cast<double>(levyBetaTimes10) / 10.0;
}

/*
 *
 * Equation (6) auxiliary sigma formula:
*/
double PEAO::levySigma(double beta) const
{
    const double numerator =
        std::tgamma(1.0 + beta) * std::sin(M_PI * beta / 2.0);

    const double denominator =
        std::tgamma((1.0 + beta) / 2.0) * beta * std::pow(2.0, (beta - 1.0) / 2.0);

    return std::pow(numerator / denominator, 1.0 / beta);
}

/*
 *
 * Equation (6):
 */
double PEAO::levyScalar(double beta)
{
    const double sigma = levySigma(beta);

    static thread_local std::mt19937 generator(std::random_device{}());
    std::normal_distribution<double> distMu(0.0, sigma);
    std::normal_distribution<double> distV(0.0, 1.0);

    const double mu = distMu(generator);
    double v = distV(generator);

    if (std::fabs(v) < 1e-12)
        v = (v >= 0.0) ? 1e-12 : -1e-12;

    return mu / std::pow(std::fabs(v), 1.0 / beta);
}

std::vector<double> PEAO::levyVector(int dimensionValue, double beta)
{
    std::vector<double> values(dimensionValue);

    for (int i = 0; i < dimensionValue; ++i)
        values[i] = levyScalar(beta);

    return values;
}

void PEAO::updateLeadersForSubpop(int subPopIndex)
{
    const int start = subPopIndex * sub_size;
    const int end = std::min(start + sub_size, population);

    if (end <= start)
        return;

    double localBestFitness = std::numeric_limits<double>::max();
    double localWorstFitness = -std::numeric_limits<double>::max();

    int localBestIdx = start;
    int localWorstIdx = start;

    for (int i = start; i < end; ++i)
    {
        const double currentFitness = fitnessArray[i];

        if (currentFitness < localBestFitness)
        {
            localBestFitness = currentFitness;
            localBestIdx = i;
        }

        if (currentFitness > localWorstFitness)
        {
            localWorstFitness = currentFitness;
            localWorstIdx = i;
        }
    }

    bestF2x[subPopIndex] = localBestFitness;
    worstF2x[subPopIndex] = localWorstFitness;
    bestIndex[subPopIndex] = localBestIdx;
    worstIndex[subPopIndex] = localWorstIdx;
    bestSamply[subPopIndex] = samples[localBestIdx];
    worstSamply[subPopIndex] = samples[localWorstIdx];
}

void PEAO::updateAllLeaders(void)
{
    for (int k = 0; k < subPopulation; ++k)
        updateLeadersForSubpop(k);

    for (int k = 0; k < subPopulation; ++k)
    {
        if (bestF2x[k] < bF2x)
        {
            bF2x = bestF2x[k];
            bSamply = bestSamply[k];
        }
    }
}

/*
 *
 * Equation (2):
*/
std::vector<double> PEAO::createCandidate1(
    const std::vector<double> &currentPosition,
    const std::vector<double> &globalBestPosition,
    double adaptiveFactorValue)
{
    std::vector<double> candidate(dimension);
    const double ri = uniform01();

    for (int j = 0; j < dimension; ++j)
    {
        candidate[j] =
            (globalBestPosition[j] - currentPosition[j]) +
            ri * std::sin(adaptiveFactorValue * currentPosition[j]);
    }

    return candidate;
}

/*
 *
 * Equation (5):
*/
std::vector<double> PEAO::buildDifferenceVector(void)
{
    std::vector<double> differenceVector(dimension, 0.0);

    if (population <= 1)
        return differenceVector;

    int p = rand() % population;
    int q = rand() % population;

    while (q == p && population > 1)
        q = rand() % population;

    for (int j = 0; j < dimension; ++j)
        differenceVector[j] = samples[p][j] - samples[q][j];

    return differenceVector;
}

/*
 *
 * Equation (4):
*/
std::vector<double> PEAO::createCandidate2A(
    const std::vector<double> &currentPosition,
    const std::vector<double> &globalBestPosition,
    const std::vector<double> &differenceVector,
    double adaptiveFactorValue)
{
    std::vector<double> candidate(dimension);
    const double beta = levyBeta();
    const std::vector<double> levyStep = levyVector(dimension, beta);

    for (int j = 0; j < dimension; ++j)
    {
        const double scA = randomDouble(enzymeConcentration, 1.0);

        candidate[j] =
            currentPosition[j] +
            levyStep[j] * differenceVector[j] +
            adaptiveFactorValue * scA * (globalBestPosition[j] - currentPosition[j]);
    }

    return candidate;
}

/*
 *
 * Equation (7):
 *
*/
std::vector<double> PEAO::createCandidate2B(
    const std::vector<double> &currentPosition,
    const std::vector<double> &globalBestPosition,
    const std::vector<double> &differenceVector,
    double adaptiveFactorValue)
{
    std::vector<double> candidate(dimension);
    const double beta = levyBeta();
    const double levyStep = levyScalar(beta);
    const double scB = randomDouble(enzymeConcentration, 1.0);

    for (int j = 0; j < dimension; ++j)
    {
        candidate[j] =
            currentPosition[j] +
            levyStep * differenceVector[j] +
            adaptiveFactorValue * scB * (globalBestPosition[j] - currentPosition[j]);
    }

    return candidate;
}



/*
 *
 * Strategy 1: Optimal Replacement
*/
void PEAO::communicationStrategy1(void)
{
    for (int g = 0; g < subPopulation; ++g)
    {
        const int idxWorst = worstIndex[g];
        if (idxWorst < 0 || idxWorst >= population)
            continue;

        samples[idxWorst] = bSamply;
        fitnessArray[idxWorst] = bF2x;
    }
}

/*
 *
 * Strategy 2: Optimal Mean Replacement
*/
void PEAO::communicationStrategy2(void)
{
    std::vector<double> meanBest(dimension, 0.0);

    for (int g = 0; g < subPopulation; ++g)
        for (int j = 0; j < dimension; ++j)
            meanBest[j] += bestSamply[g][j];

    if (subPopulation > 0)
        for (int j = 0; j < dimension; ++j)
            meanBest[j] /= static_cast<double>(subPopulation);

    const double meanBestFitness = myProblem->statFunmin(meanBest);

    for (int g = 0; g < subPopulation; ++g)
    {
        const int idxWorst = worstIndex[g];
        if (idxWorst < 0 || idxWorst >= population)
            continue;

        samples[idxWorst] = meanBest;
        fitnessArray[idxWorst] = meanBestFitness;
    }
}

/*
 *
 * Strategy 3: Circular Optimal Replacement
 *
*/
void PEAO::communicationStrategy3(void)
{
    if (subPopulation <= 1)
        return;

    std::vector<std::vector<double>> oldBestSamples = bestSamply;
    std::vector<double> oldBestFitness = bestF2x;

    for (int g = 0; g < subPopulation; ++g)
    {
        const int previousGroup = (g - 1 + subPopulation) % subPopulation;
        const int idxWorst = worstIndex[g];

        if (idxWorst < 0 || idxWorst >= population)
            continue;

        samples[idxWorst] = oldBestSamples[previousGroup];
        fitnessArray[idxWorst] = oldBestFitness[previousGroup];
    }
}

/*
 *
 * Algorithm 1:
 *
*/
void PEAO::communicate(void)
{
    int selectedStrategy = communicationStrategyMode;

    if (selectedStrategy == 0)
        selectedStrategy = 1 + (rand() % 3);

    if (selectedStrategy == 1)
        communicationStrategy1();
    else if (selectedStrategy == 2)
        communicationStrategy2();
    else
        communicationStrategy3();

    updateAllLeaders();
}

void PEAO::init()
{
    termination = terminationMethod;

    dimension = myProblem->getDimension();
    population = getParam("population").getValue().toInt();
    maxIterations = getParam("maxiters").getValue().toInt();
    localSearchRate = getParam("localsearchRate").getValue().toDouble();
    subPopulation = getParam("subPopulation").getValue().toInt();
    subPopEnable = getParam("subPopEnable").getValue().toInt();
    communicationThreshold = getParam("communicationThreshold").getValue().toInt();
    enzymeConcentration = getParam("enzymeConcentration").getValue().toDouble();
    levyBetaTimes10 = getParam("levyBetaTimes10").getValue().toInt();
    propagationMethod = getParam("propagationMethod").getValue();
    prop = getParam("prop").getValue().toInt();
    propagationNumber = getParam("propagationNumber").getValue().toInt();
    propagationRate = getParam("propagationRate").getValue().toInt();
    communicationStrategyMode = getParam("communicationStrategyMode").getValue().toInt();
    sub_size = population / subPopulation;
    iteration = 0;
    bF2x = std::numeric_limits<double>::max();
    bSamply.resize(dimension);
    fitnessArray.clear();
    samples.clear();
    threadDoublebox.resize(subPopulation);
    threadSimilarity.resize(subPopulation);
    bestF2x.resize(subPopulation);
    worstF2x.resize(subPopulation);
    bestIndex.resize(subPopulation);
    worstIndex.resize(subPopulation);
    bestSamply.resize(subPopulation);
    worstSamply.resize(subPopulation);
    bestF2xOld.resize(subPopulation);
    similarityCurrentCountBest.resize(subPopulation);

    /*
     *
     * Equation (1):
   */
    sampleFromProblem(population, samples, fitnessArray);
    updateAllLeaders();
}

void PEAO::step()
{
    iteration++;

    for (int k = 0; k < subPopulation; ++k)
        bestF2xOld[k] = bestF2x[k];

    const double AFt = adaptiveFactor();

    #pragma omp parallel for num_threads(subPopulation)
    for (int g = 0; g < subPopulation; ++g)
    {
        const int start = g * sub_size;
        const int end = (g == subPopulation - 1) ? population : std::min(start + sub_size, population);

        for (int i = start; i < end; ++i)
        {
            Data currentPosition = samples[i];
            double currentFitness = fitnessArray[i];

            Data candidate1 =
                createCandidate1(currentPosition, bSamply, AFt);
            Data differenceVector =
                buildDifferenceVector();

            Data candidate2A =
                createCandidate2A(currentPosition, bSamply, differenceVector, AFt);

            Data candidate2B =
                createCandidate2B(currentPosition, bSamply, differenceVector, AFt);

             double fitnessCandidate1 = myProblem->statFunmin(candidate1);

            double fitnessCandidate2A = myProblem->statFunmin(candidate2A);
            double fitnessCandidate2B = myProblem->statFunmin(candidate2B);

            Data candidate2;
            double fitnessCandidate2;

            if (fitnessCandidate2A < fitnessCandidate2B)
            {
                candidate2 = candidate2A;
                fitnessCandidate2 = fitnessCandidate2A;
            }
            else
            {
                candidate2 = candidate2B;
                fitnessCandidate2 = fitnessCandidate2B;
            }

            Data updatedCandidate;
            double updatedCandidateFitness;

      if (fitnessCandidate1 < fitnessCandidate2)
            {
                updatedCandidate = candidate1;
                updatedCandidateFitness = fitnessCandidate1;
            }
            else

            {
                updatedCandidate = candidate2;
                updatedCandidateFitness = fitnessCandidate2;
            }

            #pragma omp critical
            {

                if (updatedCandidateFitness< currentFitness)

                {
                    samples[i] = updatedCandidate;
                    fitnessArray[i] = updatedCandidateFitness;
                }

                if (fitnessArray[i] < bF2x)
                {
                    bF2x = fitnessArray[i];
                    bSamply = samples[i];
                }
            }
        }
        if (prop && iteration % propagationRate==0 && subPopulation > 1)
            propagate();
    }


    if (communicationThreshold > 0 && (iteration % communicationThreshold) == 0)
        communicate();

    for (int i = 0; i < static_cast<int>(samples.size()); ++i)
    {
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

    updateAllLeaders();
}

bool PEAO::checkSubCluster(int subCluster)

{
    termination = terminationMethod;

    double diff = fabs(bestF2xOld.at(subCluster) - bestF2x.at(subCluster));

    if (termination == "doublebox" &&
        threadDoublebox[subCluster].terminate(diff))
    {
        printf("subCluster %d terminated by doublebox at iter=%d diff=%g\n",
              subCluster, iteration, diff);
        return true;
    }

    if (termination == "similarity" &&
        threadSimilarity[subCluster].terminate(diff))
    {
        printf("subCluster %d terminated by similarity at iter=%d diff=%g\n",
              subCluster, iteration, diff);
        return true;
    }

    if (iteration >= maxIterations)
    {
        printf("subCluster %d terminated by maxIterations at iter=%d\n",
               subCluster, iteration);
        return true;
    }

    return false;
}

bool PEAO::terminated()
{
    int completedSubPopulations = 0;

    for (int i = 0; i < subPopulation; ++i)
        if (checkSubCluster(i))
            completedSubPopulations++;

    return completedSubPopulations >= subPopulation;
}
void PEAO::propagate()
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

void PEAO::propagateBetween(int sender, int receiver)
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



void PEAO::done()
{
    bF2x = localSearch(bSamply);

    after = std::chrono::system_clock::now();
    const auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(after - before);

    const long long ms = milliseconds.count();
    std::cout << "Duration: " << static_cast<double>(ms) / 1000.0 << " sec" << std::endl;
}

void PEAO::showDebug()
{

}

PEAO::~PEAO()
{
}
