#include <METHODS/mewoa.h>

MEWOA::MEWOA()
{
    before = std::chrono::system_clock::now();
    addParam(Parameter("rate", "5", "Number of iteration of exchange information "));
    addParam(Parameter("subPopulation", "10", "The number of subelements."));
    addParam(Parameter("population", "200", "The number of elements."));
    addParam(Parameter("maxiters", 200, 10, 10000, "Maximum iters"));
    addParam(Parameter("localsearchRate", 0.00, 0.0, 1.0, "Localsearch rate"));
    addParam(Parameter("a_min", 0.0, -5.0, 5.0, "Coefficient a_min"));
    addParam(Parameter("a_max", 2.0, -5.0, 5.0, "Coefficient a_max"));
    addParam(Parameter("b", 1.0, -5.0, 5.0, "Typical value for the shape of the logarithmic spiral"));
}

void MEWOA::init()
{
    rate = getParam("rate").getValue().toInt();
    subPopulation = getParam("subPopulation").getValue().toInt();
    dimension = myProblem->getDimension();
    population = getParam("population").getValue().toInt();
    maxIterations = getParam("maxiters").getValue().toInt();
    localSearchRate = getParam("localsearchRate").getValue().toDouble();
    a_min = getParam("a_min").getValue().toDouble();
    a_max = getParam("a_max").getValue().toDouble();
    b = getParam("b").getValue().toDouble();
    iteration = 0;
    bestF2x = 1e+100;
    sampleFromProblem(population, samples, fitnessArray);
    for (int i = 0; i < population; i++)
    {
        Data x = samples.at(i);
        double y = fitnessArray.at(i);
        if (i == 0 || y < bestF2x)
        {
            bestF2x = y;
            bestSamply = x;
        }
    }
    bestInSub.resize(subPopulation);
    for (int i = 0; i < subPopulation; i++)
    {
        bestInSub.at(i).resize(dimension);
    }
}
double MEWOA::rand_double(double min, double max)
{
    return min + (max - min) * (rand() / (double)RAND_MAX);
}
void MEWOA::exchange_info(vector<vector<double>> &population, int sub_size)
{
    for (int i = 0; i < sub_size; ++i)
    {
        int idx1 = rand() % population.size();
        int idx2 = rand() % population.size();
        swap(population[idx1], population[idx2]);
    }
}

void MEWOA::step()
{
    iteration++;
    double a = a_max - (double)iteration / maxIterations * (a_max - a_min); // Συντελεστής που μειώνεται γραμμικά
    double minValue;
    int pos = 0;
    int sub_size = population / subPopulation;
    for (int k = 0; k < subPopulation; k++)
    {
        int start = k * sub_size;
        int end = start + sub_size;
        for (int i = start; i < end; i++)
        {
            double r1 = rand_double(0, 1);
            double r2 = rand_double(0, 1);
            double A = 2 * a * r1 - a; // Συντελεστής A
            double C = 2 * r2;         // Συντελεστής C

            vector<double> D(dimension);
            vector<double> new_position(dimension);

            if (abs(A) < 1)
            { // Φάση εκμετάλλευσης
                for (int j = 0; j < dimension; ++j)
                {
                    D[j] = abs(C * bestInSub[k][j] - samples[i][j]);
                    new_position[j] = bestInSub[k][j] - A * D[j];
                }
            }
            else
            { // Φάση εξερεύνησης
                int random_whale = start + rand() % sub_size;
                for (int j = 0; j < dimension; ++j)
                {
                    D[j] = abs(C * samples[random_whale][j] - samples[i][j]);
                    new_position[j] = samples[random_whale][j] - A * D[j];
                }
            }
            // Βελτίωση της λύσης με σπειροειδή κίνηση
            double l = rand_double(-1, 1);
            double p = rand_double(0, 1);
            if (p < 0.5)
            {
                for (int j = 0; j < dimension; ++j)
                {
                    double distance_to_best = abs(bestInSub[k][j] - samples[i][j]);
                    new_position[j] = distance_to_best * exp(b * l) * cos(2 * M_PI * l) + bestInSub[k][j];
                }
            }

            if (!myProblem->isPointIn(new_position))
            {
                new_position = samples.at(i);
                // continue;
            }
            minValue = myProblem->statFunmin(new_position);
            if (minValue < fitnessArray.at(i))
            {
                samples.at(i) = new_position;
                fitnessArray.at(i) = minValue;
                bestInSub[k]=new_position;
                if (minValue < bestF2x)
                {
                    bestF2x = minValue;
                    bestSamply = new_position;
                    pos = i;
                }
            }
        }
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
    }//Μηχανισμός Ανταλλαγής
    if (iteration % rate == 0)
        exchange_info(samples, sub_size);
}

bool MEWOA::terminated()
{
    QString term = terminationMethod;
    if (iteration >= maxIterations)
        return true;
    if (term == "maxiters")
        return iteration >= maxIterations;
    else if (term == "doublebox")
        return doubleBox.terminate(bestF2x);
    else if (term == "similarity")
        return similarity.terminate(bestF2x);
    return false;
}
void MEWOA::showDebug()
{
    bool debug = getParam("opt_debug").getValue() == "yes" ? true : false;
    if (debug)
        methodLogger->printMessage(QString::asprintf("MEWOA. Iteration=%5d BEST VALUE=%20.10lg", iteration, bestF2x));
}
void MEWOA::done()
{

    bestF2x = localSearch(bestSamply);
    showDebug();
    after = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
    auto ms = milliseconds.count();
}

MEWOA::~MEWOA()
{
}
