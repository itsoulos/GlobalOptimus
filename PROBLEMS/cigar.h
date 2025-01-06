#ifndef CIGARPROBLEM_H
#define CIGARPROBLEM_H

# include <OPTIMUS/problem.h>

class Cigar : public Problem
{
private:
		int mydimension;
public:
    Cigar();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    ~Cigar();
};

#endif 
