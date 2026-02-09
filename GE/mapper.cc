# include <stdio.h>
# include <stdlib.h>
# include <GE/mapper.h>
# include <math.h>

Mapper::Mapper(int d)
{
	dimension = d;
	parser.resize(0);
	vars="x1";
	for(int i=1;i<dimension;i++)
	{
		vars=vars+",";
		char str[10];
		sprintf(str,"x%d",i+1);
		vars=vars+str;
	}	
    xx=new double[dimension];
}

bool	Mapper::setExpr(vector<string> &s)
{
	if(parser.size()!=s.size()) 
	{
	        parser.resize(s.size());
		for(int i=0;i<parser.size();i++) 
		{
			delete parser[i];
			parser[i] = new FunctionParser();

		}
	}
	for(int i=0;i<s.size();i++) 
	{
        if(parser[i]->Parse(s[i],vars)!=-1) return false;
	}
    return true;
}

int	Mapper::map(Data &x,Data &x1)
{

	for(int i=0;i<x.size();i++) xx[i]=x[i];
    for(int i=0;i<parser.size();i++)
	{
		x1[i]=parser[i]->Eval(xx);
        if(isnan(x1[i]) || isinf(x1[i])) {return 0;}
        if(parser[i]->EvalError()) {return 0;}
		//if(fabs(x1[i])>1e+2) {delete[] xx;return 0;}
	}
	return 1;
}

bool    Mapper::mapDataset(Dataset *original,Dataset *mappedDataset)
{
    int count= original->count();
    Data newPoint;newPoint.resize(parser.size());
    Matrix xall  = original->getAllXpoint();
    for(int i=0;i<count;i++)
    {
        Data xpoint = xall[i];
        double ypoint = original->getYPoint(i);
        if(!map(xpoint,newPoint)) return false;
        mappedDataset->addPoint(newPoint,ypoint);
    }
    return true;
}

bool    Mapper::mapAndReplaceDataset(Dataset *original,Dataset *mappedDataset)
{
    int count= original->count();
    Data newPoint;newPoint.resize(parser.size());
    Matrix xall  = original->getAllXpoint();
    mappedDataset->clearPoints();
    for(int i=0;i<count;i++)
    {
        Data xpoint = xall[i];
        double ypoint = original->getYPoint(i);
        if(!map(xpoint,newPoint)) return false;
       // mappedDataset->setPoint(i,newPoint,ypoint);
        mappedDataset->addPoint(newPoint,ypoint);
    }

    return true;
}

Mapper::~Mapper()
{
	for(int i=0;i<parser.size();i++) delete parser[i];
    delete[] xx;
}
