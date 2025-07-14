#include "airbf.h"
AiRbf::AiRbf()
    :Problem(1)
{
    addParam(Parameter("rbf_nodes","1","Number of rbf nodes"));
    addParam(Parameter("rbf_factor","3.0","Rbf Scale factor"));
}

RBFDerivatives AiRbf::parameterGradients(const Vector& x)
{
    RBFDerivatives grads;
    grads.dW.resize(numCenters);
    grads.dC.resize(numCenters, Vector(inputDim));
    grads.dSigma.resize(numCenters);

    for (size_t i = 0; i < numCenters; ++i) {
        const Vector& c = centers[i];
        double sigma = sigmas[i];
        double phi = gaussianRBF(x, c, sigma);
        grads.dW[i] = phi;

        // ∂y/∂c_ij
        for (size_t j = 0; j < inputDim; ++j) {
            grads.dC[i][j] = weights[i] * phi * (x[j] - c[j]) / (sigma * sigma);
        }

        // ∂y/∂σ_i
        double dist2 = 0.0;
        for (size_t j = 0; j < inputDim; ++j)
            dist2 += (x[j] - c[j]) * (x[j] - c[j]);
        grads.dSigma[i] = weights[i] * phi * (dist2) / (sigma * sigma * sigma);
    }

    return grads;
}

void    AiRbf::kMeans(const Matrix& X, size_t K,
                      Matrix& centers, vector<int>& assignments,
                      Vector& sigmas, vector<int>& teamElements)
{
    size_t N = X.size();
        size_t dim = X[0].size();
        centers.resize(K, Vector(dim));
        assignments.resize(N);
        sigmas.resize(K, 1.0);
        teamElements.resize(K);


        // Αρχικοποίηση τυχαίων κέντρων
        srand(1);//(unsigned) time(0));
        for (size_t k = 0; k < K; ++k)
            centers[k] = X[rand() % N];

        bool changed;
        int maxIters = 100;
        for (int iter = 0; iter < maxIters; ++iter) {
            changed = false;

            for(int i=0;i<K;i++)
                teamElements[i]=0;

            // Ανάθεση σε clusters
            for (size_t i = 0; i < N; ++i) {
                double minDist = numeric_limits<double>::max();
                int bestCluster = -1;
                for (size_t k = 0; k < K; ++k) {
                    double dist = euclideanDistance(X[i], centers[k]);
                    if (dist < minDist) {
                        minDist = dist;
                        bestCluster = k;
                    }
                }
                if (assignments[i] != bestCluster) {
                    assignments[i] = bestCluster;
                    changed = true;
                }
            }

            // Ενημέρωση κέντρων
            Matrix newCenters(K, Vector(dim, 0.0));
            vector<int> counts(K, 0);
            for (size_t i = 0; i < N; ++i) {
                int k = assignments[i];
                for (size_t d = 0; d < dim; ++d)
                    newCenters[k][d] += X[i][d];
                counts[k]++;
            }
            for (size_t k = 0; k < K; ++k) {
                if (counts[k] > 0)
                    for (size_t d = 0; d < dim; ++d)
                        newCenters[k][d] /= counts[k];
                else
                    newCenters[k] = X[rand() % N]; // επανεκκίνηση κενών clusters
            }

            centers = newCenters;

            if (!changed) break;
        }

        // Υπολογισμός σ^2 (variance) κάθε cluster
        for (size_t k = 0; k < K; ++k) {
            double sum = 0.0;
            int count = 0;
            for (size_t i = 0; i < N; ++i) {
                if (assignments[i] == k) {
                    sum += pow(euclideanDistance(X[i], centers[k]), 2);
                    count++;
                }
            }
    const double epsilon = 1e-6;
    teamElements[k]=count;
    sigmas[k] = (count > 0) ? max(sqrt(sum / count), epsilon) : 1.0;
    }
}

Vector  AiRbf::gaussianRBFGradient(const Vector& x, const Vector& c, double sigma)
{
    double rbf = gaussianRBF(x, c, sigma);
     Vector grad(x.size());
     for (size_t i = 0; i < x.size(); ++i)
         grad[i] = - (x[i] - c[i]) / (sigma * sigma) * rbf;
     return grad;
}

double  AiRbf::gaussianRBF(const Vector& x, const Vector& c, double sigma)
{
    double sum = 0.0;
    for (size_t i = 0; i < x.size(); ++i)
        sum += (x[i] - c[i]) * (x[i] - c[i]);
    return exp(-sum / (2 * sigma * sigma));
}

double  AiRbf::gaussianRBF(double *x, const Vector& c, double sigma)
{
    double sum = 0.0;
    for (size_t i = 0; i < trainDataset->dimension(); ++i)
        sum += (x[i] - c[i]) * (x[i] - c[i]);
    return exp(-sum / (2 * sigma * sigma));
}


double  AiRbf::euclideanDistance(const Vector& a, const Vector& b)
{
    assert(a.size() == b.size());
    double sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i)
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    return sqrt(sum);
}

void    AiRbf::initModel()
{
    int nodes        = getParam("rbf_nodes").getValue().toInt();

    weights.resize(nodes);
    int d = trainDataset->dimension();
    int k = (d*nodes)+
            nodes+nodes;
    numCenters = nodes;
    inputDim = d;
    setDimension(k);
    left.resize(k);
    right.resize(k);

    //kmeans to estimate the range of margins
    vector<Data> xpoint = trainDataset->getAllXpoint();
    vector<int> teamElements;
    vector<int> assignments;
    //kmeans??
    kMeans(xpoint,numCenters,centers,assignments,sigmas,teamElements);
    double scale_factor = 3.0;
    if(contains("rbf_factor"))
    {
        scale_factor = getParam("rbf_factor").getValue().toDouble();
    }
    int icount = 0;
    //centers bounds
    for(int i=0;i<nodes;i++)
    {
        for(int j=0;j<d;j++)
        {
            double dv = centers[i][j];
            left[icount]= -scale_factor * fabs(dv);
            right[icount]= scale_factor * fabs(dv);
            icount++;
        }
    }

    double dmin=1e+100,dmax=0.0;
    for(int i=0;i<nodes;i++)
    {
        dmin = sigmas[i]<dmin?sigmas[i]:dmin;
        dmax = sigmas[i]>dmax?sigmas[i]:dmax;
    }
    //variances bounds
    for(int i=0;i<nodes;i++)
    {
        left[icount]=  0.01;
        right[icount]= scale_factor * sigmas[i];
        if(right[icount]<left[icount])
        {
            double t = right[icount];
            right[icount]=left[icount];
            left[icount]=t;
        }

        icount++;
    }
    //weights bounds
    double y = trainDataset->maxy();
    for(int i=0;i<nodes;i++)
    {
        left[icount]=  -scale_factor * y;
        right[icount]= scale_factor * y;

        icount++;
    }

}

void    AiRbf::init(QJsonObject &params)
{
    QString trainName = params["model_trainfile"].toString();
    QString testName =  params["model_testfile"].toString();
    setParam("model_trainfile",trainName);
    setParam("model_testfile",testName);
    loadTrainSet();
    loadTestSet();
    initModel();
}

QJsonObject AiRbf::done(Data &x)
{
    setParameters(x);
    double tr=getTrainError();
    QJsonObject xx;
    double tt=getTestError(testDataset);
    double tc=getClassTestError(testDataset);
    xx["trainError"]=tr;
    xx["testError"]=tt;
    xx["classError"]=tc;
    return xx;
}

double  AiRbf::funmin(Data &x)
{
    setParameters(x);
    double f =  getTrainError();
    return f;
}

Data    AiRbf::gradient(Data &x)
{
    Data g;
    setParameters(x);
    g.resize(x.size());
    RBFDerivatives d = parameterGradients(x);
    int icount = 0;
    //centers
    for(int i=0;i<d.dC.size();i++)
    {
        for(int j=0;j<d.dC[i].size();j++)
        {
            g[icount++]=d.dC[i][j];
        }
    }
    //variances
    for(int i=0;i<d.dSigma.size();i++)
    {
        g[icount++]=d.dSigma[i];
    }
    //weights
    return g;
}

void    AiRbf::setParameters(Data &x)
{
    int icount =0;
    int nodes        = getParam("rbf_nodes").getValue().toInt();
    int d = trainDataset==NULL?1:trainDataset->dimension();

    weights.resize(nodes);
    centers.resize(nodes);
    for(int i=0;i<centers.size();i++)
        centers[i].resize(d);
    sigmas.resize(nodes);
    for(int i=0;i<nodes;i++)
    {
        for(int j=0;j<d;j++)
            centers[i][j]=x[icount++];

    }
    for(int i=0;i<nodes;i++)
        sigmas[i]=x[icount++];
    for(int i=0;i<nodes;i++)
    {
        weights[i]=x[icount++];
    }
}

void    AiRbf::getParameters(Data &x)
{
    int icount =0;
    int d = trainDataset->dimension();
    int nodes = weights.size();
    for(int i=0;i<nodes;i++)
    {
        for(int j=0;j<d;j++)
            x[icount++]=centers[i][j];

    }
    for(int i=0;i<nodes;i++)
        x[icount++]=sigmas[i];
    for(int i=0;i<nodes;i++)
        x[icount++]=weights[i];
}

double  AiRbf::getOutput(Data &x)
{
    return getOutput(x.data());
}

double  AiRbf::getOutput(double *x)
{
    double out = 0.0;
    for (size_t i = 0; i < numCenters; ++i)
        out += weights[i] * gaussianRBF(x, centers[i], sigmas[i]);
    return out;
}
AiRbf::~AiRbf()
{

}
