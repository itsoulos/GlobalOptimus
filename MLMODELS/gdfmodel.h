#ifndef GDFMODEL_H
#define GDFMODEL_H
# include <MLMODELS/model.h>
# include <GE/cprogram.h>
# include <GE/population.h>
# include <OPTIMUS/parameterlist.h>

/**
 * @brief The GdfModel class Implements the GDF data fitting model.
 */
class GdfModel : public Model
{
private:
    Cprogram    *program;
    Population  *pop;
public:
    /**
     * @brief GdfModel The constructor of the model.
     */
    GdfModel();
    /**
     * @brief getOutput
     * @param x
     * @return the value estimated at x.
     */
    virtual     double  getOutput(Data &x);
    /**
     * @brief trainModel Trains the model using Grammatical Evolution.
     */
    virtual     void    trainModel();
    virtual ~GdfModel();
};

#endif // GDFMODEL_H
