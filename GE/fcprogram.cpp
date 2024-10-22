#include "fcprogram.h"

FcProgram::FcProgram(int dim,Model *m,Dataset *orig,int nf)
    :Cprogram(dim,m)
{
    pattern_dimension = dim;
    nfeatures = nf;
    mapper = new Mapper(dim);
    originalDataset = orig;
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
    Dataset *mappedDataset = new Dataset();
    if(!mapper->setExpr(slist)) return 1e+100;
    if(!mapper->mapDataset(originalDataset,mappedDataset)) 
    {
	    delete mappedDataset;
	    return 1e+100;
    }
    currrentModel->setTrainSet(mappedDataset);
    srand(currrentModel->getModelSeed());
    currrentModel->setModelSeed(1);
    currrentModel->initModel();
    currrentModel->trainModel();
    double value = currrentModel->getTrainError();
    delete mappedDataset;
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
    delete mapper;
}
