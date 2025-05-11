#include "eo.h"
#include <algorithm>
#include <cmath>
#include <iostream>

EO::EO()
{
    before = chrono::system_clock::now();
    addParam(Parameter("population", "100", "The number of sound sources."));
    addParam(Parameter("maxiters", 100, 10, 10000, "Maximum iterations"));
    addParam(Parameter("localsearchRate", 0.01, 0.00, 1.0, "Local search rate"));
    addParam(Parameter("initialDecay", 0.9, 0.1, 1.0, "Initial echo decay factor"));
    addParam(Parameter("finalDecay", 0.1, 0.01, 0.5, "Final echo decay factor"));
    addParam(Parameter("reflectionFactor", 0.5, 0.1, 1.0, "Reflection factor towards best solution"));
    addParam(Parameter("technique", "0", "Optimization technique (0-2)"));
    addParam(Parameter("reject", 0, 0, 10, "Rejection mechanism."));
}

void EO::init()
{
    dimension = myProblem->getDimension();
    population = getParam("population").getValue().toInt();
    maxIterations = getParam("maxiters").getValue().toInt();
    localSearchRate = getParam("localsearchRate").getValue().toDouble();
    initialDecay = getParam("initialDecay").getValue().toDouble();
    finalDecay = getParam("finalDecay").getValue().toDouble();
    reflectionFactor = getParam("reflectionFactor").getValue().toDouble();
    technique = getParam("technique").getValue().toInt();
    reject = getParam("reject").getValue().toInt();

    iteration = 0;
    bestFitness = 1e+100;
    worstFitness = 1e-100;
    memoryHits = 0;

    sampleFromProblem(population, positions, fitnessArray);
    bestEchoes.resize(population, vector<double>(dimension));
    echoMemory.clear();
    updateBestSolution();
}

double EO::randomDouble(double min, double max)
{
    return min + (max - min) * (rand() / (double)RAND_MAX);
}

vector<double> EO::generateEcho(const vector<double> &source, double currentDecay)
{
    vector<double> echo(dimension);
    Data lower = myProblem->getLeftMargin();
    Data upper = myProblem->getRightMargin();

    for (int j = 0; j < dimension; j++)
    {
        double reflection = (bestSample[j] - source[j]) * reflectionFactor;
        double noise = (randomDouble(0, 1) * 2 - 1) * (1 - currentDecay) * (upper[j] - lower[j]) * 0.1;
        int n = 1; // 1: with noise, 0: without noise
        if (n == 1)
            echo[j] = source[j] + reflection + noise;
        else
            echo[j] = source[j] + reflection;

        // Boundary handling
        if (echo[j] < lower[j])
            echo[j] = lower[j] + randomDouble(0, 1) * (upper[j] - lower[j]) * 0.01;
        if (echo[j] > upper[j])
            echo[j] = upper[j] - randomDouble(0, 1) * (upper[j] - lower[j]) * 0.01;
    }

    return echo;
}

double EO::approximateFitness(const vector<double> &x, const vector<double> &best, double decay)
{
    double distance = 0.0;
    for (int j = 0; j < x.size(); j++)
    {
        distance += (x[j] - best[j]) * (x[j] - best[j]);
    }
    return distance * decay;
}

bool EO::MDM(const vector<double> &sample)
{
    int bestCount = 0;
    int worstCount = 0;
    for (size_t d = 0; d < dimension; ++d)
    {
        double diffBest = abs(sample.at(d) - bestSample.at(d));
        double diffWorst = abs(sample.at(d) - worstSample.at(d));
        if (diffBest < diffWorst)
        {
            bestCount++;
        }
        else if (diffWorst < diffBest)
        {
            worstCount++;
        }
    }
    return bestCount > worstCount;
}

bool EO::isInMemory(const vector<double> &solution, double &storedFitness)
{
    const double tolerance = 0.5;
    for (size_t i = 0; i < echoMemory.size(); ++i)
    {
        bool match = true;
        for (size_t j = 0; j < solution.size(); ++j)
        {
            if (fabs(solution[j] - echoMemory[i][j]) > tolerance)
            {
                match = false;
                break;
            }
        }
        if (match)
        {
            storedFitness = echoMemoryFitness[i];
            return true;
        }
    }
    return false;
}

void EO::updateBestSolution()
{
    for (int i = 0; i < population; i++)
    {
        if (i == 0 || fitnessArray.at(i) < bestFitness)
        {
            bestFitness = fitnessArray.at(i);
            bestSample = positions.at(i);
        }
        if (i == 0 || fitnessArray.at(i) > worstFitness)
        {
            worstFitness = fitnessArray.at(i);
            worstSample = positions.at(i);
        }
    }
}

void EO::step()
{
    iteration++;
    double currentDecay = initialDecay - (initialDecay - finalDecay) * (iteration / static_cast<double>(maxIterations));

    switch (technique)
    {
    case 0:
    {

        for (int i = 0; i < population; i++)
        {
            vector<double> echo = generateEcho(positions[i], currentDecay);
            double echoFitness;
            if (reject == 1)
            {
                if (!MDM(echo))
                {
                    echoFitness = myProblem->statFunmin(echo);
                }
            }
            else
            {
                echoFitness = myProblem->statFunmin(echo);
            }
            if (echoFitness < fitnessArray[i])
            {
                positions[i] = echo;
                fitnessArray[i] = echoFitness;
            }
        }
        break;
    }

    case 1:
    { // Προσεγγιστικές Αξιολογήσεις
        for (int i = 0; i < population; i++)
        {
            vector<double> echo = generateEcho(positions[i], currentDecay);

            double approxFit = approximateFitness(echo, bestSample, currentDecay);
            if (approxFit < fitnessArray[i] * 1.2) // 20%
            {
                double echoFitness;
                if (reject == 1)
                {
                    if (!MDM(echo))
                    {
                        echoFitness = myProblem->statFunmin(echo);
                    }
                }
                else
                {
                    echoFitness = myProblem->statFunmin(echo);
                }

                if (echoFitness < fitnessArray[i])
                {
                    positions[i] = echo;
                    fitnessArray[i] = echoFitness;
                }
            }
        }
        break;
    }

    case 2: // Μνήμη Αντιλάλων

    {
        for (int i = 0; i < population; i++)
        {
            vector<double> echo = generateEcho(positions[i], currentDecay);
            double echoFitness;

            // Έλεγχος μνήμης πρώτα
            double memoryFitness;
            if (isInMemory(echo, memoryFitness))
            {
                memoryHits++;
                if (memoryFitness < fitnessArray[i])
                {
                    positions[i] = echo;
                    fitnessArray[i] = memoryFitness;
                }
                continue;
            }

            // Αξιολόγηση μόνο αν δεν βρέθηκε στη μνήμη
            if (reject == 1)
            {
                if (!MDM(echo))
                {
                    echoFitness = myProblem->statFunmin(echo);
                }
            }
            else
            {
                echoFitness = myProblem->statFunmin(echo);
            }

            // Αποθήκευση στη μνήμη
            if (echoMemory.size() >= maxMemorySize)
            {
                echoMemory.erase(echoMemory.begin());
                echoMemoryFitness.erase(echoMemoryFitness.begin());
            }
            echoMemory.push_back(echo);
            echoMemoryFitness.push_back(echoFitness);

            if (echoFitness < fitnessArray[i])
            {
                positions[i] = echo;
                fitnessArray[i] = echoFitness;
            }
        }
        break;
    }
    }

    updateBestSolution();

    for (int i = 0; i < population; i++)
    {
        if (localSearchRate > 0.0 && randomDouble(0, 1) < localSearchRate)
        {
            fitnessArray[i] = localSearch(positions[i]);
        }
    }
}

bool EO::terminated()
{
    QString term = terminationMethod;
    if (iteration >= maxIterations)
        return true;
    if (term == "maxiters")
        return iteration >= maxIterations;
    else if (term == "doublebox")
        return doubleBox.terminate(bestFitness);
    else if (term == "similarity")
        return similarity.terminate(bestFitness);
    return false;
}

void EO::showDebug()
{
    bool debug = getParam("opt_debug").getValue() == "yes" ? true : false;
    if (debug)
    {
        QString techName;
        switch (technique)
        {
        case 0:
            techName = "Clean Echo";
            break;
        case 1:
            techName = "Approx. Evaluations";
            break;
        case 2:
            techName = "Echo Memory";
            break;
        default:
            techName = "Unknown";
        }
        methodLogger->printMessage(QString::asprintf("EO (%s). Iter=%d Best=%.10g MemHits=%zu",
                                                     techName.toStdString().c_str(), iteration, bestFitness, memoryHits));
    }
}

void EO::done()
{

    after = chrono::system_clock::now();
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(after - before);
    auto ms = milliseconds.count();

    cout << "EO Optimization finished" << endl;
    cout << "Technique: " << technique << endl;
    cout << "Duration: " << (double)ms / 1000.0 << " sec" << endl;
    cout << "Memory hits: " << memoryHits << endl;
    cout << "Best fitness: " << bestFitness << endl;
}

EO::~EO()
{
}
