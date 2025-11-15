#ifndef NNCMODEL_H
#define NNCMODEL_H
# include <MLMODELS/model.h>
# include <GE/nncprogram.h>
# include <GE/population.h>
# include <OPTIMUS/parameterlist.h>
# include <MLMODELS/neuralparser.h>
# include <MLMODELS/mlpproblem.h>
# include <MLMODELS/converter.h>
# include <METHODS/bfgs.h>
# include <METHODS/lbfgs.h>
# include <METHODS/adam.h>
# include <METHODS/genetic.h>
# include <METHODS/mbfgs.h>
class NNCModel : public Model
{
private:
    NncProgram      *program;
    Population      *pop;
    NeuralParser    *parser;
    MlpProblem      *trialProblem;
    bool            use_balancedclass=false;
public:
    NNCModel();
    double            preTrain(vector<int> &result);
    virtual         double  getOutput(Data &x);
    virtual         double  getOutput(double *x);
    void            localSearchItem(int pos);
    virtual void    trainModel();
    virtual          ~NNCModel();
    virtual double      getTrainError();
};

#endif // NNCMODEL_H
