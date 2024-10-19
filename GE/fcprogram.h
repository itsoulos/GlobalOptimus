#ifndef FCPROGRAM_H
#define FCPROGRAM_H
# include <GE/cprogram.h>
# include <GE/mapper.h>

class FcProgram :public Cprogram
{
private:
    vector<int> pstring;
    vector<int> pgenome;
    int     pattern_dimension;
    int     nfeatures;
    Mapper  *mapper;
    Dataset *mappedDataset;
public:
    FcProgram(int dim,Model *m,int nf);
    virtual double	fitness(vector<int> &genome);
    int getPatternDimension() const;
    Mapper  *getMapper();
    ~FcProgram();
};

#endif // FCPROGRAM_H
