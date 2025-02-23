#include <METHODS/saop.h>

SAOP::SAOP()
{
    before = std::chrono::system_clock::now();
    addParam(Parameter("prop", 0, 0, 1, "0 or 1"));
    addParam(Parameter("population", "100", "The number of elements."));
    addParam(Parameter("maxiters", 100, 10, 10000, "Maximum iters"));
    addParam(Parameter("localsearchRate", 0.01, 0.00, 1.0, "Localsearch rate"));
    addParam(Parameter("subPopulation", "10", "The number of subelements."));
    addParam(Parameter("propagationNumber", 0, 0, 50, "Number of positions to propagate."));
    QStringList propagationMethod;
    propagationMethod<<"1to1"<<"1toN"<<"Nto1"<<"NtoN";
    addParam(Parameter("propagationMethod", propagationMethod[0],
                       propagationMethod,
                       "Propagation strategy: 1to1, 1toN, Nto1, NtoN"));
    addParam(Parameter("subPopEnable", "1", "Number of subpopulations required to terminate."));
    addParam(Parameter("propagationRate", "10", "Number of positions to propagate."));
}

void SAOP::init()
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

    printf("prop=%d\tpropagationNumber=%d\tpropagationRate=%d\tpropagationMethod=%s\n",prop
           ,propagationNumber
           ,propagationRate
           ,propagationMethod.toStdString().c_str());

    iteration = 0;
    bestF2x.resize(subPopulation);
    bestF2xOld.resize(subPopulation);
    bSamply.resize(dimension);
    similarityCurrentCountBest.resize(subPopulation);
    for (int k = 0; k < subPopulation; k++)
    {
        bestF2x.at(k) = 1e+100;
        bestF2xOld.at(k) = 1e+100;
        similarityCurrentCountBest.at(k) = 0;
    }
    bF2x = 1e+100;
    smellIntensity.resize(population);
    evaporationRate = 0.01;
    sampleFromProblem(population, samples, fitnessArray);
    bestSamply.resize(subPopulation);
    sub_size = population / subPopulation;
    for (int k = 0; k < subPopulation; k++)
    {
        int start = k * sub_size;
        int end = start + sub_size;
        for (int i = start; i < end; i++)
        {
            Data x = samples.at(i);
            double y = fitnessArray.at(i);
            if (i == 0 || y < bestF2x.at(k))
            {
                bestF2x.at(k) = y;
                bestSamply.at(k) = x;
            }
            if (i == 0 || y < bF2x)
            {
                bF2x = y;
                bSamply = x;
            }
            smellIntensity.at(i) = calculateSmellIntensity(fitnessArray.at(i));
        }
    }
}

double SAOP::randomDouble(double min, double max)
{
    return min + (max - min) * (rand() / (double)RAND_MAX);
}

// Υπολογισμός έντασης οσμής
double SAOP::calculateSmellIntensity(double fitnessValue)
{
    // Αντιστρέφουμε τη συνάρτηση για να δώσουμε προτεραιότητα σε χαμηλότερες τιμές
    return (double)1.0 / (1.0 + fitnessValue);
}

// Εξασθένηση της οσμής
void SAOP::evaporateSmellIntensity()
{
    for (int i = 0; i < population; i++)
    {
        smellIntensity.at(i) *= (1.0 - evaporationRate);
    }
}


void SAOP::propagate()
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

void SAOP::propagateBetween(int sender, int receiver)
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

        samples[worstIndex] = samples[bestIndex];
        fitnessArray[worstIndex] = fitnessArray[bestIndex];
    }
}

void SAOP::step()
{
    iteration++;
    double minValue;
    int pos = 0;

    evaporateSmellIntensity();
#pragma omp parallel for num_threads(subPopulation)
    for (int k = 0; k < subPopulation; k++)
    {
        int start = k * sub_size;
        int end = min(start + sub_size, population); // Προστασία out-of-bounds

        for (int i = start; i < end; i++)
        {
            double y;
            y = fitnessArray[i];
            vector<double> newPosition(dimension);

            for (int j = 0; j < dimension; j++)
            {
                // Κατευθυνόμενη μετακίνηση προς την καλύτερη θέση
                double direction = randomDouble(0.0, 1.0) * smellIntensity[i] * (bestSamply[k][j] - samples[i][j]);

                // Στοχαστική κίνηση για εξερεύνηση
                double randomWalk = randomDouble(-1.0, 1.0);

                // Συνδυασμός κατευθυνόμενης και στοχαστικής κίνησης
                newPosition[j] = samples[i][j] + direction + randomWalk;

                if (!myProblem->isPointIn(newPosition))
                {
                    newPosition = samples.at(i);
                    continue;
                }
            }
#pragma omp critical
{
            minValue = myProblem->statFunmin(newPosition);

            if (minValue < fitnessArray.at(i))
            {
                // printf("aaaa\n");
                samples.at(i) = newPosition;
                fitnessArray.at(i) = minValue;
                smellIntensity.at(i) = calculateSmellIntensity(minValue);
            }

            if (minValue < bestF2x.at(k))
            {
                // printf("aaaa\n");
                bestF2x.at(k) = minValue;
                bestSamply.at(k) = newPosition;
            }
            if (minValue < bF2x)
            {
                bF2x = minValue;
                bSamply = newPosition;
                pos = i;
            }
}
        }
        if (prop && iteration % propagationRate==0 && subPopulation > 1)
            propagate();
    }
    for (int i = 0; i < samples.size(); i++)
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
bool SAOP::checkCluster(int subCluster)
{
    double difference = fabs((bestF2xOld.at(subCluster)) - bestF2x.at(subCluster));
    // printf("bestF2x=%f bestF2x=%f different=%f\n", bestF2x.at(subCluster), bestF2xOld.at(subCluster), difference);
    bestF2xOld.at(subCluster) = bestF2x.at(subCluster);
    if (difference < 1e-6)
        similarityCurrentCountBest.at(subCluster)++;
    else
        similarityCurrentCountBest.at(subCluster) = 0;
    if (similarityCurrentCountBest.at(subCluster) > 5)
        return true;
    return false;
}
bool SAOP::terminated()
{
    int c = 0;
    for (int k = 0; k < subPopulation; k++)
        if (this->checkCluster(k))
            c++;
    return iteration >= maxIterations || c >= subPopEnable;
}
void SAOP::showDebug()
{

}
void SAOP::done()
{

    bF2x = localSearch(bSamply);
    // showDebug();
    after = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
    auto ms = milliseconds.count();
    std::cout << "Douration: " << (double)ms / 1000.0 << " sec" << std::endl;
}

SAOP::~SAOP()
{
}
