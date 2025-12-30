#ifndef USERMETHOD_H
#define USERMETHOD_H

#include <OPTIMUS/optimizer.h>
#include <vector>
#include <cmath>

class UserMethod : public Optimizer
{
private:
    // Παράμετροι
    int popSize;
    int maxIters;
    double tol;

    // Μεταβλητές κατάστασης
    std::vector<std::vector<double>> population;
    std::vector<double> fitness;

    int bestIdx;
    int worstIdx;
    int currentIter;

    // Αποθήκευση του καλύτερου σημείου
    std::vector<double> globalBestX;
    double globalBestY;

    // Βοηθητικές συναρτήσεις
    void findBestWorst();
    double randDouble(double min, double max);
    void enforceBounds(std::vector<double> &x);

public:
    UserMethod();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    virtual void done();
    virtual ~UserMethod();
};

#endif // USERMETHOD_H
