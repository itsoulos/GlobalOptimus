#ifndef GDFMODEL_H
#define GDFMODEL_H
# include <MLMODELS/model.h>
# include <GE/cprogram.h>
# include <GE/population.h>
# include <OPTIMUS/parameterlist.h>

class GdfModel : public Model
{
private:
    Cprogram    *program;
    Population  *pop;
public:
    GdfModel();
    virtual     double  getOutput(Data &x);
    void        trainModel();
    virtual ~GdfModel();
};

#endif // GDFMODEL_H
