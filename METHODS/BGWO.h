#ifndef BGWO_H
#define BGWO_H
# include <OPTIMUS/collection.h>
# include <OPTIMUS/optimizer.h>

class BGWO : public Optimizer
{
private:
    Data Alpha_pos,Beta_pos,Delta_pos;
    vector<Data> Positions;
    Data fitnessArray;
    double Alpha_score,Beta_score,Delta_score;
    int SearchAgents_no, Max_iter;
    double localSearchRate;
    int iter;
    double k;
    double sumMean;
public:

    BGWO();

    virtual void init();

    virtual void step();

    virtual bool terminated();

    virtual void showDebug();

    virtual void done();
    virtual ~BGWO();
};

#endif
