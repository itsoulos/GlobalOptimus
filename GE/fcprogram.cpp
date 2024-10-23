#include "fcprogram.h"

FcProgram::FcProgram(int dim,Model *m,Dataset *orig,int nf)
    :Cprogram(dim,m)
{
    pattern_dimension = dim;
    nfeatures = nf;
    mapper = new Mapper(dim);
    originalDataset = orig;
    mappedDataset=NULL;
}

double  FcProgram::fitness(vector<int> &genome)
{
    vector<int> pgenome;
    int psize = genome.size()/nfeatures;
    pgenome.resize(psize);
    vector<string> slist;
    for(int i=0;i<nfeatures;i++)
    {
        for(int j=0;j<psize;j++)
            pgenome[j]=genome[i*psize+j];
        int redo = 0;
        string st = printRandomProgram(pgenome,redo);
        if(redo>=REDO_MAX) return 1e+100;
        slist.push_back(st);
    }

    if(!mapper->setExpr(slist)) return 1e+100;
    bool createFlag = false;
    if(mappedDataset==NULL)
    {
        mappedDataset = new Dataset;
        createFlag = true;
    }
    else
    if(mappedDataset->dimension()!=slist.size())
    {
        delete mappedDataset;
        mappedDataset = new Dataset;
        createFlag=true;
    }
    if(createFlag)
    {
        if(!mapper->mapDataset(originalDataset,mappedDataset))
        {
            return 1e+100;
        }
    }
    else
    {
        if(!mapper->mapAndReplaceDataset(originalDataset,mappedDataset))
        {
            return 1e+100;
        }
    }

    currrentModel->setTrainSet(mappedDataset);
    srand(currrentModel->getModelSeed());
    currrentModel->setModelSeed(1);
    currrentModel->initModel();
    currrentModel->trainModel();
    double value = currrentModel->getTrainError();
    return value;
}

int FcProgram::getPatternDimension() const
{
    return pattern_dimension;
}

Mapper  *FcProgram::getMapper()
{
    return mapper;
}

FcProgram::~FcProgram()
{
    if(mappedDataset!=NULL)
        delete mappedDataset;
    delete mapper;
}
