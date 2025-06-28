#include "mbfgs.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;
MBfgs::MBfgs()
{
    haveInitialized = false;
    addParam(Parameter("mbfgs_iters",2001,1,10000,"Maximum number of iterations"));
}

Data    MBfgs::matVecMul(const Matrix& A, const Data& x)
{
    Data result(x.size(), 0.0);
    for (size_t i = 0; i < A.size(); ++i)
        for (size_t j = 0; j < x.size(); ++j)
            result[i] += A[i][j] * x[j];
    return result;
}

double  MBfgs::dot(const Data& a, const Data& b)
{
    double sum = 0;
    for (size_t i = 0; i < a.size(); ++i)
        sum += a[i] * b[i];
    return sum;
}

Data    MBfgs::scalarMul(const Data& v, double scalar)
{
    Data result = v;
    for (auto& val : result) val *= scalar;
    return result;
}

Data    MBfgs::vecSub(const Data& a, const Data& b)
{
    Data result = a;
    for (size_t i = 0; i < a.size(); ++i)
        result[i] -= b[i];
    return result;
}

Data    MBfgs::vecAdd(const Data& a, const Data& b)
{
    Data result = a;
    for (size_t i = 0; i < a.size(); ++i)
        result[i] += b[i];
    return result;
}

Matrix MBfgs::updateHessian(const Matrix& H, const Data& s, const Data& y)
{
    double rho = 1.0 / dot(y, s);
    size_t n = s.size();

    Matrix I(n, Data(n, 0.0));
    for (size_t i = 0; i < n; ++i)
        I[i][i] = 1.0;

    // Outer products
    Matrix outer_sy(n, Data(n));
    Matrix outer_ys(n, Data(n));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j) {
            outer_sy[i][j] = s[i] * y[j];
            outer_ys[i][j] = y[i] * s[j];
        }

    // H_new = (I - rho * s*y^T) * H * (I - rho * y*s^T) + rho * s*s^T
    Matrix term1(n, Data(n));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j) {
            double sum = 0.0;
            for (size_t k = 0; k < n; ++k)
                for (size_t l = 0; l < n; ++l)
                    sum += (I[i][k] - rho * s[i] * y[k]) * H[k][l] * (I[l][j] - rho * y[l] * s[j]);
            term1[i][j] = sum;
        }

    Matrix ssT(n, Data(n));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            ssT[i][j] = s[i] * s[j] * rho;

    Matrix H_new(n, Data(n));
    for (size_t i = 0; i < n; ++i)
        for (size_t j = 0; j < n; ++j)
            H_new[i][j] = term1[i][j] + ssT[i][j];

    return H_new;
}

void MBfgs::init()
{
    if(!haveInitialized)
    {
        xpoint.resize(myProblem->getDimension());
        xpoint = myProblem->getSample();
        ypoint = myProblem->statFunmin(xpoint);
    }
    haveInitialized = false;
    iter = 0;
    x = xpoint;
    size_t n = xpoint.size();
    termflag = false;
    grad = myProblem->gradient(x);

    H.resize(n);
    for(int i=0;i<n;i++)
        H[i].resize(n);

    for (size_t i = 0; i < n; ++i) H[i][i] = 1.0;

    printf("START %20.10lg\n",myProblem->statFunmin(xpoint));
}

void MBfgs::step()
{

    iter++;


    Data p = scalarMul(matVecMul(H, grad), -1);
    Data x_new = vecAdd(x, p);
    //check for nan/inf
    for(int i=0;i<(int)x_new.size();i++)
    {
        if(std::isnan(x_new[i]) || std::isinf(x_new[i]))
        {
            termflag =true;
            printf("is nan\n");
             return;
        }
       /* if(!myProblem->isPointIn(x_new))
        {
            termflag =true;
            printf("out of bounds \n");
             return;
        }*/
    }
    Data grad_new = myProblem->gradient(x_new);

    Data s = vecSub(x_new, x);
    Data y = vecSub(grad_new, grad);

    cout << "Iteration " << iter  << ": x = ("
        << fixed << setprecision(6) << x_new[0] << ", " << x_new[1]
        << "), f(x) = " << myProblem->funmin(x_new) << endl;

    if (sqrt(dot(s, s)) < tol && iter>=5) {
            cout << "Converged.\n";
            termflag = true;
            xpoint = x_new;
            return;
        }

    H = updateHessian(H, s, y);
    x = x_new;
    grad = grad_new;
    xpoint = x;
}

bool MBfgs::terminated()
{
    int max_iters = getParam("mbfgs_iters").getValue().toInt();
    return iter>=max_iters || termflag;
}

void MBfgs::setPoint(Data &x,double &y)
{
    haveInitialized = true;
    xpoint = x;
    ypoint = y;
}

void MBfgs::getPoint(Data &x,double &y)
{
    x = xpoint;
    y = ypoint;
}


MBfgs::~MBfgs()
{
    //nothing here
}
