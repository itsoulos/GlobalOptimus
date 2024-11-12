#ifndef JDELSGO_H
#define JDELSGO_H
#include <OPTIMUS/collection.h>
#include <OPTIMUS/optimizer.h>

class jDElsgo : public Optimizer {
private:
     size_t  MAX_FEs;
    int imin;
    int NP;
    size_t maxiters;
    size_t iter;
    double F;
    double CR;
    Data u;
    double besty;
    QVector<Data> agentx;
    QVector<double> agenty;
    Data bestx;
    double localSearchRate, worsty, sum;
    double lower, upper, ft;
    int tournament();
    std::chrono::time_point<std::chrono::system_clock> before, after;
int i;
size_t it;
int tsize=getParam("de_tsize").getValue().toInt();
int minindex=-1;
    Data InitRandom();
    Data jDEoneIter(int index, QVector<Data>& pop);

    Data xa, xb, xc;
    int R;
    Data trialx;
     Data trial;
     double minvalue;
      int pos;
double yy;
public:
    jDElsgo();
    ~jDElsgo();
    void init();
    void step();
    bool terminated();
    void showDebug();
    void done();
    void sampleFromProblem(int NP, QVector<Data>& agentx, QVector<double>& agenty);
};

#endif // JDELSGO_H
