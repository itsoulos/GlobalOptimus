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
    QStringList yesno;
    yesno<<"yes"<<"no";
    addParam(Parameter("rbf_usesigmaminmax",yesno[0],yesno,"Use min max for bounding of sigma params"));
    addParam(Parameter("rbf_localsearchrate",0.00,0.00,1.00,"Use local search with adam"));
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
    params.resize(dimension);
    getParameters(params);

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
        setParameters(params);

        cout << "Training completed in " << iters << " iterations. Final loss: " << fx << endl;

}


void AiRbf::trainRBFwithAdamLS(const Matrix& X, const Vector& y, int epochs ) {
    Vector params;
    params.resize(dimension);
    getParameters(params);
    size_t n = params.size();

    Vector m(n, 0.0), v(n, 0.0), grad(n);
    double beta1 = 0.9, beta2 = 0.999, eps = 1e-8;
    double lr = 1e-2;

    RBFObjective obj(*this,X, y);

    for (int t = 1; t <= epochs; ++t) {
        double loss = obj(params, grad);

        for (size_t i = 0; i < n; ++i) {
            m[i] = beta1 * m[i] + (1 - beta1) * grad[i];
            v[i] = beta2 * v[i] + (1 - beta2) * grad[i] * grad[i];

            double m_hat = m[i] / (1 - std::pow(beta1, t));
            double v_hat = v[i] / (1 - std::pow(beta2, t));

            // Γραμμική αναζήτηση με backtracking
            double alpha = lr;
            Vector trial = params;
            for (size_t j = 0; j < n; ++j)
                trial[j] -= alpha * m_hat / (std::sqrt(v_hat) + eps);
            double newLoss = obj(trial, grad);
            while (newLoss > loss && alpha > 1e-6) {
                alpha *= 0.5;
                for (size_t j = 0; j < n; ++j)
                    trial[j] = params[j] - alpha * m_hat / (std::sqrt(v_hat) + eps);
                newLoss = obj(trial, grad);
            }

            params[i] = trial[i];
        }

        if (t % 100 == 0)
            std::cout << "[Epoch " << t << "] Loss = " << loss << std::endl;
    }

    setParameters(params);
}


void AiRbf::trainRBFwithAdamLineSearch(const Matrix& X, const Vector& y,
                                int epochs, double lr_init )
{
    Vector params;
    params.resize(bestx.size());
    getParameters(params);
    size_t n = params.size();

    AdamState adamW(n);
    double beta1 = 0.9, beta2 = 0.999, eps = 1e-8;

    RBFObjective obj(*this, X, y);

    for (int epoch = 1; epoch <= epochs; ++epoch) {
            Vector grad(n);
            double loss = obj(params, grad);
            adamW.t++;

            // Υπολογισμός biased m,v
            for (size_t j = 0; j < n; ++j) {
                double g = grad[j];
                adamW.m[j] = beta1 * adamW.m[j] + (1 - beta1) * g;
                adamW.v[j] = beta2 * adamW.v[j] + (1 - beta2) * g * g;
            }

            // Bias correction
            double bias1 = 1.0 - std::pow(beta1, adamW.t);
            double bias2 = 1.0 - std::pow(beta2, adamW.t);

            // Υπολογισμός κατεύθυνσης ενημέρωσης (χωρίς scale lr)
            Vector step(n);
            for (size_t j = 0; j < n; ++j) {
                double m_hat = adamW.m[j] / bias1;
                double v_hat = adamW.v[j] / bias2;
                step[j] = - m_hat / (std::sqrt(v_hat) + eps);
            }

            // --- Γραμμική αναζήτηση ---
            double alpha = lr_init;
            Vector trial(n), grad_trial;
            double newLoss;

            while (true) {
                for (size_t j = 0; j < n; ++j)
                    trial[j] = params[j] + alpha * step[j];
                newLoss = obj(trial, grad_trial);

                if (newLoss <= loss || alpha < 1e-6)
                    break;
                alpha *= 0.5; // backtracking
            }

            // Ενημέρωση παραμέτρων
            params = trial;

            if (epoch % 100 == 0)
                std::cout << "[Epoch " << epoch << "] Loss = " << loss
                          << " (lr=" << alpha << ")" << std::endl;
        }

        setParameters(params);

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



        }

    printf("Adam loss: %lf \n",getTrainError());
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

        trainRBFwithAdamLineSearch(X,y);
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
        //srand((unsigned) time(0));
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
    QString useminmax = getParam("rbf_usesigmaminmax").getValue();


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
    	if(useminmax=="yes")
       	{
		left[icount]=  -scale_factor * dmin;//0.01;
       		right[icount]= scale_factor * dmax;//sigmas[i];
	}
	else
	{
		left[icount]=  -scale_factor * sigmas[i];
		right[icount]=  scale_factor * sigmas[i];
	}
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
        left[icount]=   -scale_factor * y;
        right[icount]=  scale_factor * y;

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
        trainWithAdam(false,X,y,0.005,5000);
	getParameters(x);
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
    double rate=getParam("rbf_localsearchrate").getValue().toDouble();
    setParameters(x);
    double f =  getTrainError();
    if(rate>=0.00 && rand()*1.0/RAND_MAX<=rate)
    {
        Matrix X= trainDataset->getAllXpoint();
        Data   y= trainDataset->getAllYPoints();
        trainWithAdam(false,X,y,0.005,1000);
	getParameters(x);
	for(int i=0;i<x.size();i++)
	{
		if(x[i]<left[i]) left[i]=x[i];
		if(x[i]>right[i]) right[i]=x[i];
	}
	f=getTrainError();
    }
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
