#ifndef EGO_H
#define EGO_H
# include <OPTIMUS/problem.h>
# include <OPTIMUS/optimizer.h>
class EGO:  public Optimizer
{

private:
    double localsearchRate;
    int maxGenerations;
        vector<vector<double>> Theseis;
    vector<double> fitness;
    vector<double> grouperBestThesi;
    double grouperBestFitness;
    vector<double> eelThesi;
    vector<double> kambili;
    vector<vector<double>> troxies;
    vector<vector<double>> fitness_old;
    vector<vector<vector<double>>>position_old;
    long D;
    double newFitness;
    double y=1e+10;
    Optimizer *local=NULL;
    QString terminationMethod;
    int SearchAgents;
    vector<double> upper;
    vector<double> lower;
    int similaritySize;
    double similarityValue;
    int similarityCount;
    double r1 ;
    double r2 ;
    double r3 ;
    double r4;
    double C1;
    double C2;
    double b;
    int t;
    double p ;
    Data bestX;
    double distance2eel;
    double X1;
     double distance2grouper;
    double X2;
    double a ;
    double starvation_rate;
    int iters;
    int generation;
    Data best;
    int ego_p;

public:
    EGO();
    virtual void init();
    double evaluate( vector<double>& solution, double& grouperBestFitness);
    virtual void step();
    virtual void   done();
    virtual bool terminated();
    void    Selection();
    void  CalcFitnessArray();
    bool Feasibility(const vector<double>& solution);
    virtual ~EGO();
};

#endif // EGO_H



