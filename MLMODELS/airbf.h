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
struct AdamState {
    Vector m, v;
    int t = 0;

    AdamState(size_t size) : m(size, 0.0), v(size, 0.0), t(0) {}
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
    void decodeParameters(const Vector& params);
     void encodeParameters(Vector& params) ;
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    virtual double  funmin(Data &x);
    virtual Data    gradient(Data &x);
    void    setParameters(Data &x);
    void    getParameters(Data &x);
    double  getOutput(Data &x);
    double  getOutput(double *x);
    void trainWithGradientDescent(const Matrix& X, const Vector& y,
                                  double lr = 0.01, int epochs = 100);
    void trainWithAdam(bool init,
                       const Matrix& X, const Vector& y,
                       double lr = 0.01, int epochs = 100,
                       double beta1 = 0.9, double beta2 = 0.999, double eps = 1e-8);
    void trainRBFwithLBFGS(const Matrix& X, const Vector& y);
    void trainRBFwithAdamLS(const Matrix& X, const Vector& y, int epochs = 1000);
    void trainRBFwithAdamLineSearch(const Matrix& X, const Vector& y,
                                    int epochs = 1000, double lr_init = 1e-2);
    virtual void trainModel();
    struct RBFObjective {
        AiRbf& rbf;
        const Matrix& X;
        const Vector& y;

        RBFObjective(AiRbf& net, const Matrix& input, const Vector& target)
            : rbf(net), X(input), y(target) {}

        double operator()(const Vector& params, Vector& grad) {
            rbf.decodeParameters(params);

            size_t n = X.size(), K = rbf.numCenters, D = rbf.inputDim;
            grad.assign(params.size(), 0.0);
            double loss = 0.0;

            Vector gradW(K, 0.0);
            Matrix gradC(K, Vector(D, 0.0));
            Vector gradS(K, 0.0);

            for (size_t i = 0; i < n; ++i) {
                Data xx = X[i];
                double y_pred = rbf.getOutput(xx);
                double err = y_pred - y[i];
                loss += err * err;

                for (size_t j = 0; j < K; ++j) {
                    double phi = rbf.gaussianRBF(X[i], rbf.centers[j], rbf.sigmas[j]);
                    gradW[j] += 2 * err * phi;
                    for (size_t d = 0; d < D; ++d) {
                        double diff = X[i][d] - rbf.centers[j][d];
                        double dphi = phi * diff / (rbf.sigmas[j] * rbf.sigmas[j]);
                        gradC[j][d] += 2 * err * rbf.weights[j] * dphi;
                    }
                    double r2 = 0.0;
                    for (size_t d = 0; d < D; ++d)
                        r2 += (X[i][d] - rbf.centers[j][d]) * (X[i][d] - rbf.centers[j][d]);
                    double dphi_sigma = phi * r2 / (rbf.sigmas[j] * rbf.sigmas[j] * rbf.sigmas[j]);
                    gradS[j] += 2 * err * rbf.weights[j] * dphi_sigma;
                }
            }

            for (size_t j = 0; j < K; ++j) gradW[j] /= n;
            for (size_t j = 0; j < K; ++j)
                for (size_t d = 0; d < D; ++d)
                    gradC[j][d] /= n;
            for (size_t j = 0; j < K; ++j) gradS[j] /= n;

            size_t idx = 0;
            for (size_t j = 0; j < K; ++j) grad[idx++] = gradW[j];
            for (size_t j = 0; j < K; ++j)
                for (size_t d = 0; d < D; ++d)
                    grad[idx++] = gradC[j][d];
            for (size_t j = 0; j < K; ++j) grad[idx++] = gradS[j];

            // Αντιστροφή πρόσημου gradient για συμβατότητα με L-BFGS ελαχιστοποίηση
            for (double& g : grad) g *= -1.0;

            double avgLoss = loss / n;
            if (!std::isfinite(avgLoss)) {
                std::cerr << "Loss is NaN or Inf!" << std::endl;
                std::exit(1);
            }
            return avgLoss;
        }
    };


    ~AiRbf();
};

#endif // AIRBF_H
