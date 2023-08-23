#ifndef GRADIENTDESCENT_H
#define GRADIENTDESCENT_H
# include <optimizer.h>
# include <linesearch.h>
# include <fibonaccisearch.h>
# include <goldensearch.h>
# include <armijosearch.h>

class GradientDescent : public Optimizer
{
public:
    int maxiters;
    int iteration;
    double eps;
    double rate;
    Data xpoint;
    double ypoint;
    LineSearch *lt;
    QString lineSearchMethod;//available values: none, armijo, golden, fibonacci
public:
    GradientDescent();
    virtual void init();
    virtual void step();
    virtual bool terminated();
    void    updaterate();
    void    updatepoint();
    virtual void showDebug();
    ~GradientDescent();
};

#endif // GRADIENTDESCENT_H
