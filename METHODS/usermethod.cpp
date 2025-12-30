#include <METHODS/usermethod.h>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <iomanip>

UserMethod::UserMethod()
{
    // Ορισμός παραμέτρων
    addParam(Parameter("popsize", "100", "Population Size"));
    addParam(Parameter("max_iters", "2000", "Maximum Iterations"));
    addParam(Parameter("tol", "1e-12", "Tolerance"));
}

UserMethod::~UserMethod()
{
}

// --- Helpers ---
double UserMethod::randDouble(double min, double max) {
    double f = (double)rand() / RAND_MAX;
    return min + f * (max - min);
}

void UserMethod::findBestWorst() {
    bestIdx = 0;
    worstIdx = 0;
    for (size_t i = 1; i < population.size(); ++i) {
        if (fitness[i] < fitness[bestIdx]) bestIdx = i;
        if (fitness[i] > fitness[worstIdx]) worstIdx = i;
    }
}

void UserMethod::enforceBounds(std::vector<double> &x) {
    int dim = myProblem->getDimension();

    std::vector<double> leftBounds = myProblem->getLeftMargin();
    std::vector<double> rightBounds = myProblem->getRightMargin();

    for (int i = 0; i < dim; ++i) {
        if (x[i] < leftBounds[i]) x[i] = leftBounds[i];
        if (x[i] > rightBounds[i]) x[i] = rightBounds[i];
    }
}

// --- Main Methods ---
void UserMethod::init()
{
    // Ανάγνωση παραμέτρων
    popSize = getParam("popsize").getValue().toInt();
    maxIters = getParam("max_iters").getValue().toInt();
    tol = getParam("tol").getValue().toDouble();

    currentIter = 0;
    int dim = myProblem->getDimension();

    population.resize(popSize, std::vector<double>(dim));
    fitness.resize(popSize);

    std::vector<double> leftBounds = myProblem->getLeftMargin();
    std::vector<double> rightBounds = myProblem->getRightMargin();

    for (int i = 0; i < popSize; ++i) {
        for (int j = 0; j < dim; ++j) {
            population[i][j] = randDouble(leftBounds[j], rightBounds[j]);
        }
        fitness[i] = myProblem->funmin(population[i]);
    }

    findBestWorst();

    // Αρχικοποίηση Global Best
    globalBestX = population[bestIdx];
    globalBestY = fitness[bestIdx];
}

void UserMethod::step()
{
    int dim = myProblem->getDimension();

    for (int i = 0; i < popSize; ++i) {
        std::vector<double> newSolution = population[i];

        for (int j = 0; j < dim; ++j) {
            double r1 = randDouble(0.0, 1.0);
            double r2 = randDouble(0.0, 1.0);

            double bestVal = population[bestIdx][j];
            double worstVal = population[worstIdx][j];
            double currentVal = population[i][j];

            // JAYA Equation
            newSolution[j] = currentVal
                             + r1 * (bestVal - std::abs(currentVal))
                             - r2 * (worstVal - std::abs(currentVal));
        }

        enforceBounds(newSolution);

        double newFitness = myProblem->funmin(newSolution);

        if (newFitness < fitness[i]) {
            population[i] = newSolution;
            fitness[i] = newFitness;
        }
    }

    currentIter++;
    findBestWorst();

    // Ενημέρωση Global Best
    if (fitness[bestIdx] < globalBestY) {
        globalBestY = fitness[bestIdx];
        globalBestX = population[bestIdx];
    }
}

bool UserMethod::terminated()
{
    if (currentIter >= maxIters) return true;
    return false;
}

void UserMethod::done()
{
    // Local Search
    double localY = localSearch(globalBestX);

    if (localY < globalBestY) {
        globalBestY = localY;
    }

    std::cout << "JAYA Best Value: "
              << std::fixed << std::setprecision(10) << globalBestY << std::endl;
}
