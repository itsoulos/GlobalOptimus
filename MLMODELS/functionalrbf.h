#ifndef FUNCTIONALRBF_H
#define FUNCTIONALRBF_H

# include <MLMODELS/dataset.h>
# include <MLMODELS/model.h>
# include <OPTIMUS/problem.h>
# include <armadillo>
#include <adept.h>

class FunctionalRbf :public Problem, Model
{
private:
    QString trainName="xy.data";
    QString testName="xy.data";
    int nodes=10;
    int dimension=0;
    vector<Data> trainx;
    Data trainy;
    vector<Data> testx;
    Data testy;
    Data dclass;
    double initialLeft=-100.0;
    double initialRight= 100.0;
    int failCount=0;
    int normalTrain=0;
    double *xinput=0;
    double *yinput=0;
    vector<int> num_of_cluster_members;

    Data centers,variances;
    Data weight;
    void Kmeans(double * data_vectors,
                vector<double> &centers,
                vector<double> &variances,
                int m, int n, int K,
                vector<int>& num_of_cluster_members);
    adept::adouble aneuronOutput( vector<adept::adouble> &x, vector<double> &patt, unsigned pattDim, unsigned offset );
    adept::adouble afunmin( vector<adept::adouble> &x, vector<double> &x1 );
public:
    FunctionalRbf();
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    virtual     double  getOutput(Data &x);
    virtual double  funmin(Data &x);
    virtual Data    gradient(Data &x);
    double neuronOutput( vector<double> &x, vector<double> &patt, unsigned pattDim, unsigned offset );
    arma::vec train( vector<double> &x,bool &ok );
    double nearestClass(double y);
    ~FunctionalRbf();
};

#endif // FUNCTIONALRBF_H
