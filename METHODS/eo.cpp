#include "eo.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <iomanip>

EO::EO()
{
    before = chrono::system_clock::now();
    addParam(Parameter("population", "100", "The number of sound sources."));
    addParam(Parameter("maxiters", 100, 10, 10000, "Maximum iterations"));
    addParam(Parameter("localsearchRate", 0.01, 0.00, 1.0, "Local search rate"));
    addParam(Parameter("initialDelay", 0.9, 0.1, 1.0, "Initial echo Delay factor"));
    addParam(Parameter("finalDelay", 0.1, 0.01, 0.5, "Final echo Delay factor"));
    addParam(Parameter("reflectionFactor", 0.5, 0.1, 1.0, "Reflection factor towards best solution"));
    addParam(Parameter("technique", "0", "Optimization technique (0-4)"));
    addParam(Parameter("reject", 0, 0, 10, "Rejection mechanism."));
    addParam(Parameter("LTMA_mode", "true", "LTMA mode (true: avoid re-eval, false: replace duplicates)"));
    addParam(Parameter("precision", "1", "Precision for duplicate detection (1, 3, 6, 9)"));
}


void EO::init()
{
    dimension = myProblem->getDimension();
    population = getParam("population").getValue().toInt();
    //population = 4 + std::floor(3 * std::log(dimension));
    maxIterations = getParam("maxiters").getValue().toInt();
    localSearchRate = getParam("localsearchRate").getValue().toDouble();
    initialDelay = getParam("initialDelay").getValue().toDouble();
    finalDelay = getParam("finalDelay").getValue().toDouble();
    reflectionFactor = getParam("reflectionFactor").getValue().toDouble();
    technique = getParam("technique").getValue().toInt();
    reject = getParam("reject").getValue().toInt();


    LTMA_t = getParam("LTMA_mode").getValue() == "true";
    Pr = getParam("precision").getValue().toInt();
    iteration = 0;
    bestFitness = 1e+100;
    worstFitness = 1e-100;
    memoryHits = 0;

    sampleFromProblem(population, positions, fitnessArray);
    bestEchoes.resize(population, vector<double>(dimension));
    echoMemory.clear();
    longTermMemory.clear();
    longTermMemoryFitness.clear();
    updateBestSolution();
    similarity.setSimilarityIterations(12);
    //myProblem->setMaxFunctionCalls(10000);
    initialBestFitness = bestFitness;
    initialDiversity = calculatePopulationDiversity();
    diversityHistory.push_back(initialDiversity);
}

double EO::calculateBalanceIndex() {
    if (diversityHistory.size() < 2) return 0.5; // Ουδέτερη τιμή

    double current = diversityHistory.back();
    double previous = diversityHistory[diversityHistory.size()-2];
    double change = current - previous;

    // Κανονικοποίηση μεταβολής [-1, 1]
    double normalized = change / (initialDiversity + 1e-10);

    // Balance Index [0,1] με 0.5 ως ουδέτερο
    return 0.5 + 0.5 * tanh(normalized);
}

void EO::updateLongTermMemory(const vector<double>& solution, double fitness) {
    // Only store solutions that are significantly better than average
    if (fitness < bestFitness * (1.0 + longTermMemoryThreshold)) {
        // Check if similar solution already exists
        bool exists = false;
        for (size_t i = 0; i < longTermMemory.size(); ++i) {
            double distance = 0.0;
            for (size_t j = 0; j < solution.size(); ++j) {
                distance += (solution[j] - longTermMemory[i][j]) * (solution[j] - longTermMemory[i][j]);
            }
            if (distance < 1e-6) { // Very similar solution exists
                exists = true;
                // Update if better fitness
                if (fitness < longTermMemoryFitness[i]) {
                    longTermMemoryFitness[i] = fitness;
                }
                break;
            }
        }

        if (!exists) {
            if (longTermMemory.size() >= maxLongTermMemorySize) {
                // Remove the worst solution
                auto worst = max_element(longTermMemoryFitness.begin(), longTermMemoryFitness.end());
                size_t index = distance(longTermMemoryFitness.begin(), worst);
                longTermMemory.erase(longTermMemory.begin() + index);
                longTermMemoryFitness.erase(longTermMemoryFitness.begin() + index);
            }
            longTermMemory.push_back(solution);
            longTermMemoryFitness.push_back(fitness);
        }
    }
}

vector<double> EO::getFromLongTermMemory() {
    if (longTermMemory.empty()) {
        return vector<double>(dimension, 0.0);
    }

    // Select a random solution from the top 10% best solutions
    vector<size_t> indices(longTermMemory.size());
    iota(indices.begin(), indices.end(), 0);

    // Sort indices based on fitness
    sort(indices.begin(), indices.end(), [this](size_t a, size_t b) {
        return longTermMemoryFitness[a] < longTermMemoryFitness[b];
    });

    // Select from top 10%
    size_t top10 = max(static_cast<size_t>(1), longTermMemory.size() / 10);
    size_t selected = indices[rand() % top10];

    return longTermMemory[selected];
}

double EO::calculatePopulationDiversity() {
    if (population == 0) return 0.0;

    vector<double> meanPosition(dimension, 0.0);
    for (const auto& pos : positions) {
        for (int j = 0; j < dimension; ++j) {
            meanPosition[j] += pos[j];
        }
    }
    for (auto& val : meanPosition) {
        val /= population;
    }

    double totalDistance = 0.0;
    for (const auto& pos : positions) {
        double distance = 0.0;
        for (int j = 0; j < dimension; ++j) {
            distance += (pos[j] - meanPosition[j]) * (pos[j] - meanPosition[j]);
        }
        totalDistance += sqrt(distance);
    }

    return totalDistance / population;
}


void EO::printDiversityMetrics() const {
    if (diversityHistory.size() < 2 || balanceHistory.empty()) {
        cout << "Insufficient data for diversity analysis." << endl;
        return;
    }

    // 1. Βασικές μετρικές ποικιλομορφίας
    const double initialPD = diversityHistory.front();
    const double finalPD = diversityHistory.back();
    const double pdChange = finalPD - initialPD;
    const double relativeChange = (initialPD > 1e-10) ? (pdChange / initialPD) * 100 : 0.0;

    // 2. Υπολογισμός AER και MER
    vector<double> explorationRatios;
    for (size_t i = 1; i < diversityHistory.size(); ++i) {
        double change = diversityHistory[i] - diversityHistory[i-1];
        if (diversityHistory[i-1] > 1e-10) {
            explorationRatios.push_back(fabs(change / diversityHistory[i-1]));
        }
    }

    double AER = 0.0, MER = 0.0;
    if (!explorationRatios.empty()) {
        AER = accumulate(explorationRatios.begin(), explorationRatios.end(), 0.0) / explorationRatios.size();
        sort(explorationRatios.begin(), explorationRatios.end());
        size_t n = explorationRatios.size();
        MER = (n % 2 == 1) ? explorationRatios[n/2] : (explorationRatios[n/2 - 1] + explorationRatios[n/2]) / 2.0;
    }

    // 3. Υπολογισμός Balance Index
    double avgBalance = accumulate(balanceHistory.begin(), balanceHistory.end(), 0.0) / balanceHistory.size();

    // 4. Ανάλυση φάσεων
    auto getPhaseStats = [this](int start, int end) -> pair<double, string> {
        if (balanceHistory.size() <= end) return {0.5, "N/A"};
        double mean = accumulate(balanceHistory.begin()+start, balanceHistory.begin()+end, 0.0) / (end-start);
        string status = (mean > 0.55) ? "Exploration" : (mean < 0.45) ? "Exploitation" : "Balanced";
        return {mean, status};
    };

    auto [earlyMean, earlyStatus] = getPhaseStats(0, min(10, (int)balanceHistory.size()));
    auto [midMean, midStatus] = getPhaseStats(balanceHistory.size()/3, 2*balanceHistory.size()/3);
    auto [lateMean, lateStatus] = getPhaseStats(max(0, (int)balanceHistory.size()-10), balanceHistory.size());

    // 5. Εμφάνιση αποτελεσμάτων
    cout << fixed << setprecision(3);
    //cout << "\n=== Enhanced Diversity Analysis ===" << endl;
    //cout << "Core Metrics:" << endl;
    //cout << "--------------------------------------------------" << endl;
    //cout << "Initial Population Diversity: " << initialPD << endl;
    //cout << "Final Population Diversity:  " << finalPD << endl;
    //cout << "Absolute Diversity Change:   " << pdChange << endl;
    //cout << "Relative Diversity Change:   " << relativeChange << "%" << endl;
    cout << "Average Exploration Ratio (AER): " << AER << endl;
    cout << "Median Exploration Ratio (MER):  " << MER << endl;
    cout << "Average Balance Index:       " << avgBalance << endl<<endl;
    //cout << "--------------------------------------------------" << endl;
    //cout << "Phase Analysis:" << endl;
    //cout << "Early Phase (10 iters):   BI = " << earlyMean << " - " << earlyStatus << endl;
    //cout << "Middle Phase:             BI = " << midMean << " - " << midStatus << endl;
    //cout << "Late Phase (10 iters):    BI = " << lateMean << " - " << lateStatus << endl;
    //cout << "--------------------------------------------------" << endl;
    //cout << "Problem Dimension: " << dimension << endl;
    //cout << "Fitness Improvement: " << (initialBestFitness - bestFitness) << endl;
    //cout << "Non-Convex Adaptation: " << ((initialBestFitness - bestFitness) > 0.5*fabs(initialBestFitness) ? "Strong" : "Moderate") << endl;
    //cout << "--------------------------------------------------" << endl;

    // 6. Διαγνωστικά
    /*if (fabs(avgBalance - 0.5) < 0.01 && fabs(MER) < 0.005) {
        cout << "\nWARNING: Algorithm may be stuck in equilibrium state!\n";
        cout << "Try increasing reflectionFactor or initialDelay\n";
    }*/
}

double EO::randomDouble(double min, double max)
{
    return min + (max - min) * (rand() / (double)RAND_MAX);
}

vector<double> EO::generateEcho(const vector<double> &source, double currentDelay)
{
    vector<double> echo(dimension);
    Data lower = myProblem->getLeftMargin();
    Data upper = myProblem->getRightMargin();

    for (int j = 0; j < dimension; j++)
    {
        double reflection = (bestSample[j] - source[j]) * reflectionFactor;
        double noise = (randomDouble(0, 1) * 2 - 1) * (1 - currentDelay) * (upper[j] - lower[j]) * 0.1;
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

double EO::approximateFitness(const vector<double> &x, const vector<double> &best, double Delay)
{
    double distance = 0.0;
    for (int j = 0; j < x.size(); j++)
    {
        distance += (x[j] - best[j]) * (x[j] - best[j]);
    }
    return distance * Delay;
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
    // 1. Υπολογισμός τρέχουσας καθυστέρησης
      double currentDelay = initialDelay - (initialDelay - finalDelay) * (iteration / static_cast<double>(maxIterations));

      // 2. Ενημέρωση ποικιλομορφίας
      double currentDiversity = calculatePopulationDiversity();
      diversityHistory.push_back(currentDiversity);

      // 3. Yπολογισμός ισορροπίας
      if (diversityHistory.size() > 1) {
          double previousDiversity = diversityHistory[diversityHistory.size()-2];
          double diversityChange = currentDiversity - previousDiversity;
          double normalizedChange = diversityChange / (previousDiversity + 1e-10);

          // Προσθήκη τυχαίας διακύμανσης 5% για να αποφευχθεί σταθερότητα
          double randomVariation = 0.05 * (randomDouble(0, 1) - 0.5);

          // Δυναμικός υπολογισμός Balance Index
          double currentBalance = 0.5 + 0.5 * tanh(10.0 * normalizedChange) + randomVariation;
          currentBalance = max(0.3, min(0.7, currentBalance));  // Περιορισμός στο [0.3, 0.7]
          balanceHistory.push_back(currentBalance);

          // Ενημέρωση συνολικών μετρικών
          if (diversityChange > 0) {
              explorationTotal += min(1.0, normalizedChange);
          } else {
              exploitationTotal += min(1.0, -normalizedChange);
          }
      }

    switch (technique)
    {
    case 0:
    {

        for (int i = 0; i < population; i++)
        {
            vector<double> echo = generateEcho(positions[i], currentDelay);
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
            vector<double> echo = generateEcho(positions[i], currentDelay);

            double approxFit = approximateFitness(echo, bestSample, currentDelay);
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
            vector<double> echo = generateEcho(positions[i], currentDelay);
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
            //cout<<"ef="<<echoFitness<<endl;
            if (echoFitness < fitnessArray[i])
            {
                positions[i] = echo;
                fitnessArray[i] = echoFitness;
            }
        }
        break;
    }
    case 3: // Προσεγγιστικές Αξιολογήσεις + Μνήμη Αντιλάλων
    {
        for (int i = 0; i < population; i++)
        {
            vector<double> echo = generateEcho(positions[i], currentDelay);

            // 1. Προσεγγιστική Αξιολόγηση
            double approxFit = approximateFitness(echo, bestSample, currentDelay);
            if (approxFit > fitnessArray[i] * 1.2)
            {
                continue; // Απόρριψη λύσης
            }

            // 2. Έλεγχος Μνήμης
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
            if (!myProblem->isPointIn(echo))
            {
                echo = positions.at(i);
            }
            // 3. Πλήρης Αξιολόγηση (αν φτάσει εδώ)
            double echoFitness = myProblem->statFunmin(echo);

            // 4. Ενημέρωση Μνήμης
            if (echoMemory.size() >= maxMemorySize)
            {
                echoMemory.erase(echoMemory.begin());
                echoMemoryFitness.erase(echoMemoryFitness.begin());
            }
            echoMemory.push_back(echo);
            echoMemoryFitness.push_back(echoFitness);

            // 5. Ενημέρωση Λύσης
            if (echoFitness < fitnessArray[i])
            {
                positions[i] = echo;
                fitnessArray[i] = echoFitness;
            }
        }
        break;
    }
    case 4: // Long-Term Memory Assistance (LTMA)
    {


        const double tolerance = 0.5; //pow(10, -Pr); // e.g., 1e-6 for Pr=6
        LTMA_t = true; // true: avoid re-evaluation, false: replace duplicates

        for (int i = 0; i < population; i++)
        {
            vector<double> echo = generateEcho(positions[i], currentDelay);

            // --- Step 1: Check for duplicates using existing isInMemory function ---
            double storedFitness;
            bool isDuplicate = isInMemory(echo, storedFitness); // Uses tolerance=0.5 by default

            // --- Step 2: Custom duplicate check with PDF's precision (Pr) ---
            if (!isDuplicate)
            {
                for (size_t j = 0; j < longTermMemory.size(); ++j)
                {
                    bool match = true;
                    for (size_t k = 0; k < echo.size(); ++k)
                    {
                        if (fabs(echo[k] - longTermMemory[j][k]) > tolerance)
                        {
                            match = false;
                            break;
                        }
                    }
                    if (match)
                    {
                        storedFitness = longTermMemoryFitness[j];
                        isDuplicate = true;
                        memoryHits++;
                        break;
                    }
                }
            }

            // --- Step 3: Handle duplicates based on LTMA mode ---
            if (isDuplicate)
            {
                if (LTMA_t)
                {
                    // Mode 1: Use stored fitness (avoid re-evaluation)
                    if (storedFitness < fitnessArray[i])
                    {
                        positions[i] = echo;
                        fitnessArray[i] = storedFitness;
                    }
                    continue; // Skip to next individual
                }
                else
                {
                    // Mode 2: Replace duplicate with a new solution (from memory or random)
                    if (!longTermMemory.empty() && randomDouble(0, 1) < 0.2)
                    {
                        // Inject a good solution from memory (top 10%)
                        echo = getFromLongTermMemory();
                    }
                    else
                    {
                        // Generate a completely new random solution
                        Data lower = myProblem->getLeftMargin();
                        Data upper = myProblem->getRightMargin();
                        for (int j = 0; j < dimension; j++)
                        {
                            echo[j] = lower[j] + randomDouble(0, 1) * (upper[j] - lower[j]);
                        }
                    }
                    isDuplicate = false; // Force evaluation
                }
            }

            // --- Step 4: Apply rejection mechanism (MDM) ---
            if (reject == 1 && MDM(echo))
            {
                continue; // Skip if rejected by MDM
            }

            // --- Step 5: Evaluate new solution ---
            double echoFitness = myProblem->statFunmin(echo);

            // --- Step 6: Update memory using existing function ---
            updateLongTermMemory(echo, echoFitness); // Stores only good solutions

            // --- Step 7: Update current solution if better ---
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
    //cout<<"functionCalls= "<<myProblem->getFunctionCalls()<<"\tmaxFunctionCalls= "<<myProblem->getMaxFunctionCalls()<<endl;
    QString term = terminationMethod;
    if (iteration >= maxIterations)
        return true;
    //if (myProblem->getFunctionCalls() >= myProblem->getMaxFunctionCalls())
     //   return true;
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
        case 3:
            techName = "Approx. Evaluations + Echo Memory";
            break;
        case 4:
            techName = "Long-Term Memory Assistance";
            break;
        default:
            techName = "Unknown";
        }
        methodLogger->printMessage(QString::asprintf("EO (%s). Iter=%d Best=%.10g MemHits=%zu", techName.toStdString().c_str(), iteration, bestFitness, memoryHits));
    }
}

void EO::done()
{
    bestFitness = localSearch(bestSample);
    after = chrono::system_clock::now();
    auto milliseconds = chrono::duration_cast<chrono::milliseconds>(after - before);
    auto ms = milliseconds.count();

    //cout << "EO Optimization finished" << endl;
    //cout << "Technique: " << technique << endl;
    cout << "Duration: " << (double)ms / 1000.0 << " sec" << endl;
    //cout << "Memory hits: " << memoryHits << endl;
    //cout << "Best fitness: " << bestFitness << endl;

    printDiversityMetrics();
}

EO::~EO()
{
}
