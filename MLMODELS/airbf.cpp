#include "airbf.h"
# include "rbf_lbfgs.h"
AiRbf::AiRbf()
    :Problem(1)
{
    addParam(Parameter("rbf_nodes","1","Number of rbf nodes"));
    addParam(Parameter("rbf_factor","3.0","Rbf Scale factor"));
    QStringList method;
    method<<"optimus"<<"rbf_gd"<<"rbf_adam"<<"rbf_bfgs";
    addParam(Parameter("rbf_train",method[0],method,"Rbf training method"));
}

void AiRbf::decodeParameters(const Vector& params) {
       size_t K = numCenters, D = inputDim;
       size_t idx = 0;
       for (size_t j = 0; j < K; ++j) weights[j] = params[idx++];
       for (size_t j = 0; j < K; ++j)
           for (size_t d = 0; d < D; ++d)
               centers[j][d] = params[idx++];
       for (size_t j = 0; j < K; ++j)
           sigmas[j] = std::max(params[idx++], 1e-3);
}

void AiRbf::encodeParameters(Vector& params) {
        size_t K = numCenters, D = inputDim;
        params.resize(K + K * D + K);
        size_t idx = 0;
        for (size_t j = 0; j < K; ++j) params[idx++] = weights[j];
        for (size_t j = 0; j < K; ++j)
            for (size_t d = 0; d < D; ++d)
                params[idx++] = centers[j][d];
        for (size_t j = 0; j < K; ++j) params[idx++] = sigmas[j];
}

void AiRbf::trainRBFwithLBFGS( const Matrix& X, const Vector& y)
{
    Vector params;
        encodeParameters(params);

        // Προσθήκη τυχαιότητας στην αρχικοποίηση αν μηδενικά
        for (double& v : params) {
            if (std::abs(v) < 1e-8) {
                v = ((rand() / (double)RAND_MAX) - 0.5) * 0.1;
            }
        }

        RBFObjective obj(*this, X, y);
        Options opts;
        opts.maxIter = 100;
        double fx;
        int iters = LBFGS::minimize(obj, params, fx, opts);
        decodeParameters(params);

        cout << "Training completed in " << iters << " iterations. Final loss: " << fx << endl;

}

void AiRbf::trainWithAdam(bool init,
                   const Matrix& X, const Vector& y,
                   double lr, int epochs,
                   double beta1, double beta2, double eps)
{
    vector<int> assignments;
    vector<int> t;
    if(init)
    {
        kMeans(X, numCenters, centers, assignments, sigmas,t);
        weights.assign(numCenters, 0.0);
    }

    size_t N = X.size();

    AdamState adamW(numCenters);
    vector<AdamState> adamC(numCenters, AdamState(inputDim));
    AdamState adamS(numCenters);

    double bestError = getTrainError();
    Data bestX=bestx;
    getParameters(bestX);

    for (int epoch = 0; epoch < epochs; ++epoch) {
        Vector dw(numCenters, 0.0);
        Matrix dc(numCenters, Vector(inputDim, 0.0));
        Vector ds(numCenters, 0.0);
        double loss = 0.0;

        for (size_t i = 0; i < N; ++i) {
            Data xx = X[i];
            double y_pred = getOutput(xx);
            double error = y_pred - y[i];
            loss += error * error;

            RBFDerivatives grads = parameterGradients(X[i]);
            for (size_t j = 0; j < numCenters; ++j) {
                dw[j] += 2 * error * grads.dW[j];
                for (size_t k = 0; k < inputDim; ++k)
                     dc[j][k] += 2 * error * grads.dC[j][k];
                     ds[j] += 2 * error * grads.dSigma[j];
                }
            }
        if(loss<=bestError)
        {
            bestError = loss;
            getParameters(bestX);

        }

            // Adam update
            adamW.t++;
            adamS.t++;
            for (size_t j = 0; j < numCenters; ++j) {
                // --- weights
                double g = dw[j] / N;
                adamW.m[j] = beta1 * adamW.m[j] + (1 - beta1) * g;
                adamW.v[j] = beta2 * adamW.v[j] + (1 - beta2) * g * g;
                double mhat = adamW.m[j] / (1 - pow(beta1, adamW.t));
                double vhat = adamW.v[j] / (1 - pow(beta2, adamW.t));
                weights[j] -= lr * mhat / (sqrt(vhat) + eps);

                // --- sigmas
                double gs = ds[j] / N;
                adamS.m[j] = beta1 * adamS.m[j] + (1 - beta1) * gs;
                adamS.v[j] = beta2 * adamS.v[j] + (1 - beta2) * gs * gs;
                double mhat_s = adamS.m[j] / (1 - pow(beta1, adamS.t));
                double vhat_s = adamS.v[j] / (1 - pow(beta2, adamS.t));
                sigmas[j] -= lr * mhat_s / (sqrt(vhat_s) + eps);
                sigmas[j] = max(sigmas[j], 1e-3);  // prevent collapse

                // --- centers
                adamC[j].t++;
                for (size_t k = 0; k < inputDim; ++k) {
                    double gc = dc[j][k] / N;
                    adamC[j].m[k] = beta1 * adamC[j].m[k] + (1 - beta1) * gc;
                    adamC[j].v[k] = beta2 * adamC[j].v[k] + (1 - beta2) * gc * gc;
                    double mhat_c = adamC[j].m[k] / (1 - pow(beta1, adamC[j].t));
                    double vhat_c = adamC[j].v[k] / (1 - pow(beta2, adamC[j].t));
                    centers[j][k] -= lr * mhat_c / (sqrt(vhat_c) + eps);
                }
            }
            if(epoch%200==0)
            cout << "Epoch " << epoch << ", Loss: " << loss << endl;
        }

    printf("ADAM BEST = %20.10lg \n",bestError);
    setParameters(bestX);
}

void AiRbf::trainWithGradientDescent(const Matrix& X, const Vector& y,
                              double lr , int epochs)
{
    vector<int> assignments;
    vector<int> t;
    kMeans(X, numCenters, centers, assignments, sigmas,t);
    weights.assign(numCenters, 0.0); // Αρχικά βάρη
    size_t N = X.size();
    for (int epoch = 0; epoch < epochs; ++epoch) {
        Vector dw(numCenters, 0.0);
        Matrix dc(numCenters, Vector(inputDim, 0.0));
        Vector ds(numCenters, 0.0);

        double loss = 0.0;

        for (size_t i = 0; i < N; ++i) {
            Data xx = X[i];
            double y_pred = getOutput(xx);
            double error = y_pred - y[i];
            loss += error * error;
            RBFDerivatives grads = parameterGradients(X[i]);

            for (size_t j = 0; j < numCenters; ++j) {
                dw[j] += 2 * error * grads.dW[j];
                for (size_t k = 0; k < inputDim; ++k)
                    dc[j][k] += 2 * error * grads.dC[j][k];
                    ds[j] += 2 * error * grads.dSigma[j];
                }
            }

            // Μέση τιμή και βήμα καθόδου
            for (size_t j = 0; j < numCenters; ++j) {
                weights[j] -= lr * dw[j] / N;
                sigmas[j]  -= lr * ds[j] / N;
                for (size_t k = 0; k < inputDim; ++k)
                    centers[j][k] -= lr * dc[j][k] / N;
            }
            if(epoch%200==0)

            cout << "Epoch " << epoch << ", Loss: " << loss  << endl;
        }

}

 void AiRbf::trainModel()
{
    QString method = getParam("rbf_train").getValue();
    Matrix X= trainDataset->getAllXpoint();
    Data   y= trainDataset->getAllYPoints();
    if(method=="optimus")
    {
        Model::trainModel();
    }
    else
    if(method == "rbf_gd")
    {

        trainWithGradientDescent(X, y, 0.005, 2000);
    }
    else
    if(method == "rbf_adam")
    {

        trainWithAdam(true,X, y, 0.05, 2000);
    }
    else
    if(method == "rbf_bfgs")
    {
        trainRBFwithLBFGS(X,y);
    }
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
        srand((unsigned) time(0));
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
        left[icount]=  -scale_factor * dmin;//0.01;
        right[icount]= scale_factor * dmax;//sigmas[i];
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
    QString method = getParam("rbf_train").getValue();
    if(method == "optimus")
    {
        setParameters(x);
        Matrix X= trainDataset->getAllXpoint();
        Data   y= trainDataset->getAllYPoints();
        trainWithAdam(false,X, y, 0.05, 5000);
    }
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
    for(int i=0;i<g.size();i++) g[i]=0.0;
    int tcount = trainDataset->count();
    Data gtemp;
    gtemp.resize(g.size());
    for(int i=0;i<tcount;i++)
    {
        Data xx = trainDataset->getXPoint(i);
        double per=getOutput(xx)-trainDataset->getYPoint(i);
    	RBFDerivatives d = parameterGradients(xx);
    	int icount = 0;
    	//centers
    	for(int j=0;j<d.dC.size();j++)
    	{
        	for(int k=0;k<d.dC[j].size();k++)
        	{
            	gtemp[icount++]=d.dC[j][k];
        	}
    	}
    	//variances
    	for(int j=0;j<d.dSigma.size();j++)
    	{
        	gtemp[icount++]=d.dSigma[j];
    	}
    	//weights
    	for(int j=0;j<d.dW.size();j++)
    	{
		gtemp[icount++]=d.dW[j];
    	}

        for(int j=0;j<dimension;j++)	g[j]+=gtemp[j]*per;
    }
    for(int j=0;j<x.size();j++) g[j]*=2.0;
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
