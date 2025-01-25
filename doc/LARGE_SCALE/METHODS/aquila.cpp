#include <METHODS/aquila.h>

Aquila::Aquila()
{
    before = std::chrono::system_clock::now();
    addParam(Parameter("population", "100", "The number of elements."));
    addParam(Parameter("maxiters", 100, 10, 10000, "Maximum iters"));
    addParam(Parameter("localsearchRate", 0.01, 0.0, 1.0, "Localsearch rate"));
    addParam(Parameter("a_min", 0.0, -5.0, 5.0, "Coefficient a_min"));
    addParam(Parameter("a_max", 2.0, -5.0, 5.0, "Coefficient a_max"));
}

void Aquila::init()
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
double Aquila::randomDouble(double min, double max)
{
    return min + (double)(max - min) * (rand() / (double)RAND_MAX);
}

void Aquila::step()
{
    iteration++;
    double alpha = a_max - (double)iteration / maxIterations * (a_max - a_min); // Συντελεστής που μειώνεται γραμμικά
    double minValue;
    int pos = 0;
    for (int i = 0; i < samples.size(); i++)
    {
        double y; 
        y = fitnessArray[i];

        vector<double> newPosition(dimension);

        // Επιλογή στρατηγικής πτήσης
        double randStrategy = randomDouble(0.0, 1.0);
        if (randStrategy < 0.25)
        {
            // Στρατηγική 1: Καθοδική πτήση (Exploration)
            for (int j = 0; j < dimension; j++)
            {
                double r1 = randomDouble(0.0, 1.0);
                double r2 = randomDouble(0.0, 1.0);
                newPosition[j] = bestSamply[j] + alpha * (r1 - 0.5) * (myProblem->getLeftMargin().at(j) - myProblem->getRightMargin().at(j)) * r2;
            }
        }
        else if (randStrategy < 0.5)
        {
            // Στρατηγική 2: Σπειροειδής πτήση
            for (int j = 0; j < dimension; j++)
            {
                double theta = randomDouble(-M_PI, M_PI);
                newPosition[j] = bestSamply[j] + alpha * sin(theta) * (samples[i][j] - bestSamply[j]);
            }
        }
        else if (randStrategy < 0.75)
        {
            // Στρατηγική 3: Στοχαστική αναζήτηση
            for (int j = 0; j < dimension; j++)
            {
                newPosition[j] = randomDouble(myProblem->getRightMargin().at(j), myProblem->getLeftMargin().at(j));
            }
        }
        else
        {
            // Στρατηγική 4: Καθοδική εκμετάλλευση
            for (int j = 0; j < dimension; j++)
            {
                double distance = fabs(samples[i][j] - bestSamply[j]);
                newPosition[j] = bestSamply[j] - alpha * distance * randomDouble(0.0, 1.0);
            }
        }

        if (!myProblem->isPointIn(newPosition))
        {
            newPosition = samples.at(i);
            // continue;
        }

        minValue = myProblem->statFunmin(newPosition);
        if (minValue < y)
        {
            samples.at(i) = newPosition;
            fitnessArray.at(i) = minValue;
            if (minValue < bestF2x)
            {
                bestF2x = minValue;
                bestSamply = newPosition;
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

bool Aquila::terminated()
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
void Aquila::showDebug()
{
    bool debug = getParam("opt_debug").getValue() == "yes" ? true : false;
    if (debug)
        methodLogger->printMessage(QString::asprintf("Aquila. Iteration=%5d BEST VALUE=%20.10lg", iteration, bestF2x));
}
void Aquila::done()
{

    bestF2x = localSearch(bestSamply);
    showDebug();
    after = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
    auto ms = milliseconds.count();
    std::cout << "Douration: " << (double)ms / 1000.0 << " sec" << std::endl;
}

Aquila::~Aquila()
{
}
