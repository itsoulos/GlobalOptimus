#include <MLMODELS/gdfmodel.h>

GdfModel::GdfModel()
{
    program = NULL;
    pop =   NULL;
    addParam(Parameter("gdf_popcount","200","Population count"));
    addParam(Parameter("gdf_popsize","100","Gdf chromosome size"));
    addParam(Parameter("gdf_popgens","200","Maximum generations for gdf"));
    addParam(Parameter("gdf_popsrate","0.90","Gdf selection rate"));
    addParam(Parameter("gdf_popmrate","0.05","Gdf mutation rate"));

}

double  GdfModel::getOutput(Data &x)
{
    if(pop==NULL) return 0.0;

    return program->Eval(x.data());
}

void    GdfModel::trainModel()
{
    loadTrainSet();
    loadTestSet();
    if(trainDataset == NULL) return;
    if(program!=NULL) delete program;
    if(pop!=NULL) delete pop;
    program = new Cprogram(trainDataset->dimension(),this);
    program->makeGrammar();
    pop = new Population(getParam("gdf_popcount").getValue().toInt(),
                         getParam("gdf_popsize").getValue().toInt(),
                         program,getModelSeed());
    pop->setSelectionRate(getParam("gdf_popsrate").getValue().toDouble());
    pop->setMutationRate(getParam("gdf_popmrate").getValue().toDouble());
    int gens = getParam("gdf_popgens").getValue().toInt();
    for(int g=1;g<=gens;g++)
    {
        printf(" generation = %d best value= %20.10lg\n",g,pop->getBestFitness());
        pop->nextGeneration();
    }
    pop->evaluateBestFitness();
}

GdfModel::~GdfModel()
{
    if(program!=NULL)
        delete program;
    if(pop!=NULL)
        delete pop;
}
