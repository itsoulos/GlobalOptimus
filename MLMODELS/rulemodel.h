#ifndef RULEMODEL_H
#define RULEMODEL_H

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
# include <GE/ruleprogram.h>
class RuleModel : public Model
{
private:
    Population      *pop;
    RuleProgram     *program;
public:
    RuleModel();
    virtual     double  getOutput(Data &x);
    void        localSearchItem(int pos);
    void        trainModel();
    virtual     ~RuleModel();
};

#endif // RULEMODEL_H
