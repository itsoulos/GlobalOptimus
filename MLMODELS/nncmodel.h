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
class NNCModel : public Model
{
private:
    NncProgram      *program;
    Population      *pop;
    NeuralParser    *parser;
    MlpProblem      *trialProblem;
public:
    NNCModel();
    virtual     double  getOutput(Data &x);
    void        localSearchItem(int pos);
    virtual void        trainModel();
    virtual     ~NNCModel();
};

#endif // NNCMODEL_H
