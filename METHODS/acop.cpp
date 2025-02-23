#include <METHODS/acop.h>

ACOP::ACOP()
{
    before = std::chrono::system_clock::now();
    addParam(Parameter("population", "100", "The number of elements."));
    addParam(Parameter("maxiters", 100, 10, 10000, "Maximum iters"));
    addParam(Parameter("localsearchRate", 0.01, 0.00, 1.0, "Localsearch rate"));
    addParam(Parameter("subPopulation", "4", "Number of subpopulations"));
    addParam(Parameter("subPopEnable", "1", "Number of subpopulations required to terminate."));
    addParam(Parameter("prop", 0, 0, 1, "0 or 1"));
    addParam(Parameter("propagationNumber", 0, 0, 50, "Number of positions to propagate."));;
    QStringList propagationMethod;
    propagationMethod<<"1to1"<<"1toN"<<"Nto1"<<"NtoN";
    addParam(Parameter("propagationMethod", propagationMethod[0],
                       propagationMethod,
                       "Propagation strategy: 1to1, 1toN, Nto1, NtoN"));
    addParam(Parameter("propagationRate", "10", "Number of positions to propagate."));
}

void ACOP::init()
{
    dimension = myProblem->getDimension();
    population = getParam("population").getValue().toInt();
    maxIterations = getParam("maxiters").getValue().toInt();
    localSearchRate = getParam("localsearchRate").getValue().toDouble();
    subPopulation = getParam("subPopulation").getValue().toInt();
    subPopEnable = getParam("subPopEnable").getValue().toInt();
    prop = getParam("prop").getValue().toInt();
    propagationNumber = getParam("propagationNumber").getValue().toInt();
    propagationMethod = getParam("propagationMethod").getValue();
    propagationRate = getParam("propagationRate").getValue().toInt();

    printf("prop=%d\tpropagationNumber=%d\tpropagationRate=%d\tpropagationMethod=%s\n",prop
           ,propagationNumber
           ,propagationRate
           ,propagationMethod.toStdString().c_str());
    //printf("propagationMethod=%s\n",propagationMethod.toUtf8().data());



    iteration = 0;
    alpha = 1.0;
    beta = 2.0;
    evaporationRate = 0.5;
    Q = 100.0;
    bestF2xAll = 1e+100;

    sub_size = population / subPopulation;
    propagationNumber = min(propagationNumber, sub_size);

    antPositions.resize(population, vector<double>(dimension));
    fitnessArray.resize(population);
    pheromones.resize(population, vector<double>(dimension, 1.0));
    bestF2x.resize(subPopulation, 1e+100);
    bestSamply.resize(subPopulation, vector<double>(dimension));
    bestF2xOld.resize(subPopulation, 1e+100);
    similarityCurrentCountBest.resize(subPopulation, 0);
    // bestAllSamply.resize(dimension);
    sampleFromProblem(population, antPositions, fitnessArray);

#pragma omp parallel for num_threads(subPopulation)
    for (int k = 0; k < subPopulation; k++)
    {
        int start = k * sub_size;
        int end = min(start + sub_size, population); // Προστασία out-of-bounds

        for (int i = start; i < end; i++)
        {
            double y = fitnessArray[i];
            if (y < bestF2x[k])
            {
                bestF2x[k] = y;
                bestSamply[k] = antPositions[i];
            }
        }
    }
}

double ACOP::randomDouble(double min, double max)
{
    return min + (max - min) * (rand() / (double)RAND_MAX);
}

void ACOP::moveAnts()
{
#pragma omp parallel for num_threads(subPopulation)
    for (int k = 0; k < subPopulation; k++)
    {
        int start = k * sub_size;
        int end = min(start + sub_size, population); // Προστασία out-of-bounds

        for (int i = start; i < end; i++)
        {
            vector<double> antPositionTemp = antPositions[i];

            for (int d = 0; d < dimension; ++d)
            {
                double total_pheromone = 0.0;
                // Υπολογισμός φερομόνων εντός του υποπληθυσμού
                for (int j = start; j < end; ++j)
                {
                    total_pheromone += pow(pheromones[j][d], alpha) * pow(1.0 / (1.0 + fitnessArray[j]), beta);
                }

                double r = randomDouble(0.0, total_pheromone);
                double cumulative = 0.0;
                for (int j = start; j < end; ++j)
                {
                    cumulative += pow(pheromones[j][d], alpha) * pow(1.0 / (1.0 + fitnessArray[j]), beta);
                    if (cumulative >= r)
                    {
                        antPositions[i][d] = antPositions[j][d];
                        break;
                    }
                }
            }
            if (!myProblem->isPointIn(antPositions[i]))
            {
                antPositions[i] = antPositionTemp;
                continue;
            }
#pragma omp critical
            {

                fitnessArray[i] = myProblem->statFunmin(antPositions[i]);
            }
        }
    }
}

void ACOP::updatePheromones()
{
#pragma omp parallel for num_threads(subPopulation)
    for (int k = 0; k < subPopulation; k++)
    {
        int start = k * sub_size;
        int end = min(start + sub_size, population); // Προστασία out-of-bounds

        for (int i = start; i < end; i++)
        {
            for (int d = 0; d < dimension; d++)
            {
                pheromones[i][d] *= (1.0 - evaporationRate);
                if (pheromones[i][d] < 1e-10)
                    pheromones[i][d] = 1e-10;
            }
        }

        for (int i = start; i < end; i++)
        {
            double pheromone_contribution = Q / (fitnessArray[i] + 1e-10);
            for (int d = 0; d < dimension; d++)
            {
#pragma omp atomic
                pheromones[i][d] += pheromone_contribution;
            }
        }
    }
}

void ACOP::step()
{
    iteration++;

    moveAnts();
    updatePheromones();

#pragma omp parallel for num_threads(subPopulation)
    for (int k = 0; k < subPopulation; k++)
    {
        int start = k * sub_size;
        int end = min(start + sub_size, population); // Προστασία out-of-bounds

        for (int i = start; i < end; i++)
        {
#pragma omp critical
            {
                double localMin = fitnessArray[i];
                if (localMin < bestF2x[k])
                {
                    bestF2x[k] = localMin;
                    bestSamply[k] = antPositions[i];
                }
                if (localSearchRate > 0.0)
                {
                    double r = rand() * 1.0 / RAND_MAX;
                    if (r < localSearchRate)
                    {
                        fitnessArray[i] = localSearch(antPositions[i]);
                    }
                }
                if (fitnessArray[i] < bestF2xAll)
                {
                    bestF2xAll = fitnessArray[i];
                    bestAllSamply = antPositions[i];
                }
            }
        }
    }


    if (prop==1 && iteration % propagationRate==0 && subPopulation > 1)
        propagate();
}

bool ACOP::terminated()
{
    // printf("subPopEnable=%d\n", subPopEnable);
    int c = 0;
    for (int k = 0; k < subPopulation; k++)
        if (this->checkCluster(k))
            c++;
    return iteration >= maxIterations || c >= subPopEnable;
}

bool ACOP::checkCluster(int subCluster)
{
    double difference = fabs((bestF2xOld.at(subCluster)) - bestF2x.at(subCluster));
     //printf("bestF2xOld.at(%d)=%f bestF2x.at(%d)=%f different=%f\n", bestF2xOld.at(subCluster), subCluster, bestF2x.at(subCluster), subCluster, difference);
    bestF2xOld.at(subCluster) = bestF2x.at(subCluster);
    if (difference < 1e-6)
        similarityCurrentCountBest.at(subCluster)++;
    else
        similarityCurrentCountBest.at(subCluster) = 0;
    if (similarityCurrentCountBest.at(subCluster) > 10)
        return true;
    return false;
}

void ACOP::propagate()
{
    if (propagationMethod == "1to1")
    {
        // Ένας τυχαίος υποπληθυσμός στέλνει σε έναν άλλο τυχαίο
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
        // Ένας τυχαίος υποπληθυσμός στέλνει σε όλους τους υπόλοιπους
        int sender = rand() % subPopulation;
        for (int receiver = 0; receiver < subPopulation; ++receiver)
        {
            if (receiver != sender)
                propagateBetween(sender, receiver);
        }
    }
    else if (propagationMethod == "Nto1")
    {
        // Όλοι οι υποπληθυσμοί στέλνουν σε έναν τυχαίο
        int receiver = rand() % subPopulation;
        for (int sender = 0; sender < subPopulation; ++sender)
        {
            if (sender != receiver)
                propagateBetween(sender, receiver);
        }
    }
    else if (propagationMethod == "NtoN")
    {
        // Όλοι οι υποπληθυσμοί στέλνουν σε όλους τους υπόλοιπους
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

void ACOP::propagateBetween(int sender, int receiver)
{
    int senderStart = sender * sub_size;
    int senderEnd = std::min(senderStart + sub_size, population);
    int receiverStart = receiver * sub_size;
    int receiverEnd = std::min(receiverStart + sub_size, population);

    // Ταξινόμηση fitness για τον αποστολέα
    vector<std::pair<double, int>> senderFitnessIndex;
    for (int i = senderStart; i < senderEnd; ++i)
    {
        senderFitnessIndex.emplace_back(fitnessArray[i], i);
    }
    std::sort(senderFitnessIndex.begin(), senderFitnessIndex.end());

    // Ταξινόμηση fitness για τον παραλήπτη
    vector<std::pair<double, int>> receiverFitnessIndex;
    for (int i = receiverStart; i < receiverEnd; ++i)
    {
        receiverFitnessIndex.emplace_back(fitnessArray[i], i);
    }
    std::sort(receiverFitnessIndex.begin(), receiverFitnessIndex.end(), std::greater<>());

    // Αντικατάσταση των χειρότερων του παραλήπτη με τους καλύτερους του αποστολέα
    for (int i = 0; i < propagationNumber; ++i)
    {
        int bestIndex = senderFitnessIndex[i].second;
        int worstIndex = receiverFitnessIndex[i].second;

        antPositions[worstIndex] = antPositions[bestIndex];
        fitnessArray[worstIndex] = fitnessArray[bestIndex];
        pheromones[worstIndex] = pheromones[bestIndex];
    }
}

void ACOP::showDebug()
{
}

void ACOP::done()
{
    bestF2xAll = localSearch(bestAllSamply);

    after = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
    auto ms = milliseconds.count();
    std::cout << "Duration: " << (double)ms / 1000.0 << " sec" << std::endl;
}

ACOP::~ACOP()
{
}
