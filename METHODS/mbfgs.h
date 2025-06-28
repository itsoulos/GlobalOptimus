#ifndef MBFGS_H
#define MBFGS_H
# include <OPTIMUS/optimizer.h>
using namespace std;


class MBfgs :public Optimizer
{
private:
    Data xpoint;
    double ypoint;
    bool haveInitialized;
    int iter = 0;
    double tol = 1e-6;
    Data grad,x;
    Matrix H;
    bool termflag=false;
    Data matVecMul(const Matrix& A, const Data& x);
    double dot(const Data& a, const Data& b);
    Data scalarMul(const Data& v, double scalar);
    Data vecSub(const Data& a, const Data& b);
    Data vecAdd(const Data& a, const Data& b);
    Matrix updateHessian(const Matrix& H, const Data& s, const Data& y);
public:
    MBfgs();
    /**
     * @brief init Initializes the parameters of the LBFGS.
     */
    virtual void init();
    /**
     * @brief step Performs a step of LBFGS.
     */
    virtual void step();
    /**
     * @brief terminated
     * @return true when BFGS should be terminated.
     */
    virtual bool terminated();
    /**
     * @brief setPoint Sets the initial point of the optimizer.
     * @param x
     * @param y
     */
    void    setPoint(Data &x,double &y);
    /**
     * @brief getPoint Returns the located local minimum.
     * @param x
     * @param y
     */
    void    getPoint(Data &x,double &y);
    ~MBfgs();
};

#endif // MBFGS_H
