#include "fcprogram.h"

FcProgram::FcProgram(int dim,Model *m,int nf)
    :Cprogram(dim,m)
{
    pattern_dimension = dim;
    nfeatures = nf;
    mapper = new Mapper(dim);
    mappedDataset = new Dataset();
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
        slist.push_back(st);
    }
    mapper->setExpr(slist);
    mappedDataset->clearPoints();
    Dataset *tr=currrentModel->getTrainDataset();
    mapper->mapDataset(tr,mappedDataset);
    double value = currrentModel->getTestError(mappedDataset);
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
    delete mappedDataset;
}
