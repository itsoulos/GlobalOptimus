#include <METHODS/sao.h>

SAO::SAO()
{
    before = std::chrono::system_clock::now();
    addParam(Parameter("population", "100", "The number of elements."));
    addParam(Parameter("maxiters", 100, 10, 10000, "Maximum iters"));
    addParam(Parameter("localsearchRate", 0.01, 0.00, 1.0, "Localsearch rate"));
}

void SAO::init()
{
    dimension = myProblem->getDimension();
    population = getParam("population").getValue().toInt();
    maxIterations = getParam("maxiters").getValue().toInt();
    localSearchRate = getParam("localsearchRate").getValue().toDouble();
    iteration = 0;
    bestF2x = 1e+100;
    smellIntensity.resize(population);
    evaporationRate=0.01;
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
        smellIntensity.at(i)=calculateSmellIntensity(fitnessArray.at(i));
    }

}
double SAO::randomDouble(double min, double max)
{
    return min + (max - min) * (rand() / (double)RAND_MAX);
}

// Υπολογισμός έντασης οσμής
double SAO::calculateSmellIntensity(double fitnessValue)
{
    // Αντιστρέφουμε τη συνάρτηση για να δώσουμε προτεραιότητα σε χαμηλότερες τιμές
    return (double)1.0 / (1.0 + fitnessValue);
}

// Εξασθένηση της οσμής
void SAO::evaporateSmellIntensity()
{
    for (int i = 0; i < population; i++)
    {
        smellIntensity.at(i) *= (1.0 - evaporationRate);
    }
}

void SAO::step()
{
    iteration++;
    double minValue;
    int pos = 0;
    evaporateSmellIntensity();
    for (int i = 0; i < samples.size(); i++)
    {
        double y;
        y = fitnessArray[i];
        vector<double> newPosition(dimension);

        for (int j = 0; j < dimension; j++)
        {
            // Κατευθυνόμενη μετακίνηση προς την καλύτερη θέση
            double direction = randomDouble(0.0, 1.0) * smellIntensity[i] * (bestSamply[j] - samples[i][j]);

            // Στοχαστική κίνηση για εξερεύνηση
            double randomWalk = randomDouble(-1.0, 1.0);

            // Συνδυασμός κατευθυνόμενης και στοχαστικής κίνησης
            newPosition[j] = samples[i][j] + direction + randomWalk;

            if (!myProblem->isPointIn(newPosition))
            {
                newPosition = samples.at(i);
                // continue;
            }
        }



        minValue = myProblem->statFunmin(newPosition);
        if (minValue < y)
        {
            samples.at(i) = newPosition;
            fitnessArray.at(i) = minValue;
            smellIntensity.at(i) = calculateSmellIntensity(minValue);
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

bool SAO::terminated()
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
void SAO::showDebug()
{
    bool debug = getParam("opt_debug").getValue() == "yes" ? true : false;
    if (debug)
        methodLogger->printMessage(QString::asprintf("SAO. Iteration=%5d BEST VALUE=%20.10lg", iteration, bestF2x));
}
void SAO::done()
{

    bestF2x = localSearch(bestSamply);
    showDebug();
    after = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
    auto ms = milliseconds.count();
    std::cout << "Douration: " << (double)ms / 1000.0 << " sec" << std::endl;
}

SAO::~SAO()
{
}
