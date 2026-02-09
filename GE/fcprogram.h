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
    Dataset *originalDataset;
    Dataset *mappedDataset;
    bool    useErrorPerClass = false;
public:
    FcProgram(int dim,Model *m,Dataset *orig,int nf);
    virtual double	fitness(vector<int> &genome);
    int     getPatternDimension() const;
    void    setUseErrorPerClass(bool value);
    Mapper  *getMapper();
    ~FcProgram();
};

#endif // FCPROGRAM_H
