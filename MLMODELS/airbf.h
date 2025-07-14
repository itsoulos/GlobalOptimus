#ifndef AIRBF_H
#define AIRBF_H
# include <MLMODELS/dataset.h>
# include <MLMODELS/model.h>
# include <OPTIMUS/problem.h>
# include <math.h>
using namespace std;

using Vector = vector<double>;
using Matrix = vector<Vector>;
struct RBFDerivatives {
    Vector dW;        // ∂y/∂w_i
    Matrix dC;        // ∂y/∂c_ij
    Vector dSigma;    // ∂y/∂σ_i
};

class AiRbf: public Problem, public Model
{
private:
    size_t inputDim;
    size_t numCenters;
    Matrix centers;
    Vector sigmas;
    Vector weights;
    void kMeans(const Matrix& X, size_t K, Matrix& centers, vector<int>& assignments, Vector& sigmas,vector<int>&t);
    Vector gaussianRBFGradient(const Vector& x, const Vector& c, double sigma);
    double gaussianRBF(const Vector& x, const Vector& c, double sigma);
    double gaussianRBF(double *x, const Vector& c, double sigma);

    double euclideanDistance(const Vector& a, const Vector& b);
    RBFDerivatives parameterGradients(const Vector& x) ;
public:
    AiRbf();
    virtual void initModel();
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    virtual double  funmin(Data &x);
    virtual Data    gradient(Data &x);
    void    setParameters(Data &x);
    void    getParameters(Data &x);
    double  getOutput(Data &x);
    double  getOutput(double *x);
    ~AiRbf();
};

#endif // AIRBF_H
