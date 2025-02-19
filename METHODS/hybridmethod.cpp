#include <METHODS/hybridmethod.h>

Data HybridMethod::makeChild(Data &parent1,Data &parent2)
{
    Data child;
    child.resize(dimension);
    for(int i=0;i<dimension;i++)
    {
        double alpha = rand() *1.0/RAND_MAX;
        //alpha = -0.5 + 2.0 * alpha;//[-0.5,1.5]
        double r = rDouble(0.0, 1.0);
        child.at(i) = alpha * parent1.at(i)+(1.0-alpha)*parent2.at(i);
    }
    return child;
}
vector<double> HybridMethod::findSamply(const vector<double> &A, const vector<double> &B, double t)
{
    vector<double> result(dimension);
    for (int j = 0; j < dimension; j++)
    {
        result.at(j) = A.at(j) + t * (B.at(j) - A.at(j));
    }
    return result;
}

double HybridMethod::goldenSectionSearch(const vector<double> &A, const vector<double> &B, double tol = 1e-5)
{
    const double phi = (1 + std::sqrt(5)) / 2; // Χρυσός λόγος
    double a = 0.0;
    double b = 1.0;// 1.0: ψάχνει όλο το μηκος της ευθείας ab, 0.5: μέχρι την μέση

    double c = b - (b - a) / phi;
    double d = a + (b - a) / phi;

    while (std::abs(c - d) > tol)
    {
        vector<double> Lc = findSamply(A, B, c);
        vector<double> Ld = findSamply(A, B, d);

        if (myProblem->funmin(Lc) < myProblem->funmin(Ld))
        {
            b = d;
        }
        else
        {
            a = c;
        }
        c = b - (b - a) / phi;
        d = a + (b - a) / phi;
    }
    return (b + a) / 2.0;
}

double HybridMethod::armijoSearch(const vector<double>& A, const vector<double>& B, double tol = 1e-5) {
    double alpha = 0.5;
    double beta = 0.5;
    double sigma = 0.1;

    double t = 0.0; // Ξεκινάμε από το μέσο του διαστήματος [0, 0.5]
    vector<double> x0 = findSamply(A, B, 0.0); // Το σημείο A
    vector<double> x1 = findSamply(A, B, t);   // Αρχικό σημείο αναζήτησης (το μέσο)

    while (myProblem->funmin(x1) > myProblem->funmin(x0) - sigma * t * myProblem->funmin(x0) && t > tol) {
        t *= beta; // Μειώνουμε το t σύμφωνα με το Armijo condition
        x1 = findSamply(A, B, t); // Υπολογίζουμε το νέο σημείο στην ευθεία
    }

    return t;
}
double HybridMethod::euclideanDistance(const Data &a, const Data &b)
{
    double sum = 0.0;
    for (int i = 0; i < dimension; i++)
    {
        sum += (a.at(i) - b.at(i)) * (a.at(i) - b.at(i));
    }
    return std::sqrt(sum);
}

void HybridMethod::findTwoClosestSamplesForEachSamply() {

    for (int i = 0; i < population; i++) {
        double minDistance1 = numeric_limits<double>::max();
        double minDistance2 = numeric_limits<double>::max();
        Data closestSample1, closestSample2;

        for (int j = 0; j < population; j++) {
            if (i == j) continue;

            double dist = euclideanDistance(samples.at(i), samples.at(j));

            if (dist < minDistance1) {
                minDistance2 = minDistance1;
                closestSample2 = closestSample1;

                minDistance1 = dist;
                closestSample1 = samples.at(j);
            } else if (dist < minDistance2) {
                minDistance2 = dist;
                closestSample2 = samples.at(j);
            }
        }
        closestSamples.at(i) = {closestSample1, closestSample2};
    }
}

double HybridMethod::rDouble(double low, double high)
{
    return low + (double)rand() / RAND_MAX * (high - low);
}

HybridMethod::HybridMethod()
{
    before = std::chrono::system_clock::now();
    addParam(Parameter("rejection", 0, 0, 1, "rejection 0 or 1"));
    addParam(Parameter("calculation", 1, 1, 10, "calculation method 1,2,..10."));
    addParam(Parameter("population", "1000", "The number of elements."));
    addParam(Parameter("maxiters", 200, 10, 10000, "Maximum iters"));
    addParam(Parameter("localsearchRate", 0.05, 0.0, 1.0, "Localsearch rate"));
    addParam(Parameter("linesearch", 0.01, 0.0, 1.0, "Localsearch rate"));
    QStringList lineSearch;
    lineSearch<<"goldenSectionSearch"<<"armijoSearch";
    addParam(Parameter("lineSearch",lineSearch[0], lineSearch, "Selection line search method. Available values goldenSectionSearch,armijoSearch"));
}

void HybridMethod::init()
{
    rejection = getParam("rejection").getValue().toInt();
    calculation = getParam("calculation").getValue().toInt();
    dimension = myProblem->getDimension();
    if (getParam("population").getValue() == "10n")
        population = 10 * dimension;
    else
        population = getParam("population").getValue().toInt();
    maxIterations = getParam("maxiters").getValue().toInt();
    localSearchRate = getParam("localsearchRate").getValue().toDouble();
    lineSearchMethod = getParam("lineSearch").getValue();
    iteration = 0;
    bestF2x = 1e+100;

    sampleFromProblem(population, samples, fitnessArray);
    samplesCopy = samples;
    closeSamples.resize(population);
    closeSamples = samples;
    closestSamples.resize(population);
    for (int i = 0; i < population; i++)
    {
        closestSamples.at(i).first.resize(dimension);
        closestSamples.at(i).second.resize(dimension);
    }
    for (int i = 0; i < population; i++)
    {

        closestSamples.at(i).first = samples.at(i);
        closestSamples.at(i).second = samples.at(i);
        Data x = samples.at(i);
        double y = fitnessArray.at(i);
        if (i == 0 || y < bestF2x)
        {
            bestF2x = y;
            bestSamply = x;
        }
    }  
}

void HybridMethod::step()
{
    findTwoClosestSamplesForEachSamply();

    vector<double> minLSample,trialSample;
    minLSample.resize(dimension);
    trialSample.resize(dimension);
    double minValue, t_min;
    int pos=0;
    samplesCopy = samples;
    for (int i = 0; i < samples.size(); i++)
    {
        Data x;
        double y;
        x= samples[i];
        y= fitnessArray[i];
        if (lineSearchMethod=="armijoSearch")
            t_min = armijoSearch(samples.at(i), closestSamples.at(i).first);
        else
            t_min = goldenSectionSearch(samples.at(i), closestSamples.at(i).first);

        minLSample = findSamply(samples.at(i), closestSamples.at(i).first, t_min);


        Data child = makeChild(closestSamples.at(i).first, closestSamples.at(i).second);
        Data child2 = makeChild(samples.at(i), bestSamply);
        double r = rDouble(0.0, 1.0);
        for (int j = 0; j < dimension; j++)
        {
            if (calculation==1)
                trialSample.at(j) = samples.at(i).at(j)  + r* (minLSample.at(j)-child2.at(j));
            else if (calculation==2)
                trialSample.at(j) = samples.at(i).at(j) + r * (samples.at(i).at(j) - closestSamples.at(i).first.at(j));
            else if (calculation==3)
                trialSample.at(j) = samples.at(i).at(j) + r * (closestSamples.at(i).first.at(j) - closestSamples.at(i).second.at(j));
            else if (calculation==4)
                trialSample.at(j) = samples.at(i).at(j) + r * (closestSamples.at(i).first.at(j) - minLSample.at(j));
            else if (calculation==5)
                trialSample.at(j) = samples.at(i).at(j)  + r * (samples.at(i).at(j) -  closestSamples.at(i).second.at(j) + minLSample.at(j));
            else if (calculation==6)
                trialSample.at(j) = minLSample.at(j) + r * ( bestSamply.at(j) - closestSamples.at(i).first.at(j));
            else if (calculation==7)
                trialSample.at(j) = samples.at(i).at(j) + r * ( child2.at(j) - closestSamples.at(i).first.at(j));
            else if (calculation==8)
                trialSample.at(j) = minLSample.at(j) + r * (samples.at(i).at(j) - child.at(j));
            else if (calculation==9)
                trialSample.at(j) = trialSample.at(j) = samples.at(i).at(j) + r*(closestSamples.at(i).first.at(j) - closestSamples.at(i).second.at(j)-child2.at(j))  +  r * (minLSample.at(j)-child2.at(j));

        }
        if (rejection==1)
            if (samples.at(i)==samplesCopy.at(i))
                continue;

        if (!myProblem->isPointIn(trialSample))
        {
            trialSample = samples.at(i);
            //continue;
        }


        //minValue = myProblem->funmin(trialSample);
        minValue = myProblem->statFunmin(trialSample);

        if(minValue<y)
        {
            samples.at(i)=trialSample;
            fitnessArray.at(i)=minValue;
            if(minValue<bestF2x)
            {
                bestF2x= minValue;
                bestSamply = trialSample;
                pos=i;
            }
        }
    }
//ανακατανομή
/*
    int randomX;

    for (int k=0;k<samples.size();k++)
    {
        do
        {
            randomX = rand() % samples.size();
        }while (k == randomX);
        Data rSample =samples.at(randomX);
        samples.at(k) = makeChild( samples.at(k), rSample);
        fitnessArray.at(k) = myProblem->funmin(samples.at(k));
    }
    samples.at(pos) = bestSamply;
    fitnessArray.at(pos) = bestF2x;

*/
    iteration++;

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

bool HybridMethod::terminated()
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

void HybridMethod::showDebug()
{
    bool debug = getParam("opt_debug").getValue() == "yes" ? true : false;
    if (debug)
        methodLogger->printMessage(QString::asprintf("HybridMethod. Iteration=%5d BEST VALUE=%20.10lg", iteration, bestF2x));
}

void HybridMethod::done()
{

    bestF2x = localSearch(bestSamply);
    showDebug();
    after = std::chrono::system_clock::now();
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
    auto ms = milliseconds.count();
}

HybridMethod::~HybridMethod()
{
    // nothing here
}
