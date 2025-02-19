#include <METHODS/woa.h>

WOA::WOA()
{
    before = std::chrono::system_clock::now();
    addParam(Parameter("population", "200", "The number of elements."));
    addParam(Parameter("maxiters", 200, 10, 10000, "Maximum iters"));
    addParam(Parameter("localsearchRate", 0.00, 0.0, 1.0, "Localsearch rate"));
    addParam(Parameter("a_min", 0.0, -5.0, 5.0, "Coefficient a_min"));
    addParam(Parameter("a_max", 2.0, -5.0, 5.0, "Coefficient a_max"));
    addParam(Parameter("b", 1.0, -5.0, 5.0, "Typical value for the shape of the logarithmic spiral"));
}

void WOA::init()
{
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
}
double WOA::rand_double(double min, double max)
{
    return min + (max - min) * (rand() / (double)RAND_MAX);
}

void WOA::step()
{
    iteration++;
    double a = a_max - (double)iteration / maxIterations * (a_max - a_min); // Συντελεστής που μειώνεται γραμμικά
    double minValue;
    int pos = 0;
    for (int i = 0; i < samples.size(); i++)
    {
        Data x;
        double y;
        x = samples[i];
        y = fitnessArray[i];

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
                D[j] = abs(C * bestSamply[j] - samples[i][j]);
                new_position[j] = bestSamply[j] - A * D[j];
            }
        }
        else
        { // Φάση εξερεύνησης
            int random_whale = rand() % population;
            for (int j = 0; j < dimension; ++j)
            {
                D[j] = std::abs(C * samples[random_whale][j] - samples[i][j]);
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
                double distance_to_best = std::abs(bestSamply[j] - samples[i][j]);
                new_position[j] = distance_to_best * exp(b * l) * cos(2 * M_PI * l) + bestSamply[j];
            }
        }

        if (!myProblem->isPointIn(new_position))
        {
            new_position = samples.at(i);
            // continue;
        }

        minValue = myProblem->statFunmin(new_position);
        if (minValue < y)
        {
            samples.at(i) = new_position;
            fitnessArray.at(i) = minValue;
            if (minValue < bestF2x)
            {
                bestF2x = minValue;
                bestSamply = new_position;
                pos = i;
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
    }
}

bool WOA::terminated()
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
void WOA::showDebug()
{
    bool debug = getParam("opt_debug").getValue() == "yes" ? true : false;
    if (debug)
        methodLogger->printMessage(QString::asprintf("WOA. Iteration=%5d BEST VALUE=%20.10lg", iteration, bestF2x));
}
void WOA::done()
{

    bestF2x = localSearch(bestSamply);
    showDebug();
    after = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
    auto ms = milliseconds.count();
}

WOA::~WOA()
{
}
