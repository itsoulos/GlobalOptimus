#ifndef USERMETHOD_H
#define USERMETHOD_H

#include "problem.h"
#include <string>
#include <vector>

class UserMethod
{
private:
    Problem *myProblem;

    Matrix hawks;
    std::vector<double> fit;

    Data bestx;
    double besty;
    double besty_old;

    Data L, R;

    int popsize;
    int max_iters;
    int seed;
    std::string sampling_mode; // "uniform" or "gaussian"

    int max_evals;
    double tol;
    int tol_window;
    int max_stall;

    double p_local;
    int local_every_R;
    int top_k;
    int local_steps;
    double local_lr;

    int iter;
    int stall;
    int tol_hits;

    double rand01() const;
    double randUniform(double a, double b) const;
    double randNormal(double mean, double stddev) const;

    void enforceBounds(Data &x) const;
    int bestIndex() const;

    Data levyFlight(int d, double beta) const;
    void localOptimize(Data &x, double &y);

public:
    explicit UserMethod(Problem *p);

    void setPopsize(int n);
    void setMaxIters(int it);
    void setSeed(int s);
    void setSamplingMode(const std::string &m);

    void setMaxEvals(int e);
    void setTol(double t);
    void setTolWindow(int g);
    void setMaxStall(int s);

    void setPLocal(double p);
    void setLocalEveryR(int r);
    void setTopK(int k);
    void setLocalSteps(int s);
    void setLocalLr(double lr);

    void init();
    void step();
    bool terminated();
    void done();

    void solve();

    Data getBestx() const;
    double getBesty() const;

    ~UserMethod();
};

#endif

