#ifndef DISCUSPROBLEM_H
#define DISCUSPROBLEM_H

# include <OPTIMUS/problem.h>

class Discus : public Problem
{
private:
		int mydimension;
public:
    Discus();
    double funmin(Data &x);
    Data gradient(Data &x);
    virtual void init(QJsonObject &params);
    virtual QJsonObject done(Data &x);
    ~Discus();
};

#endif 
