#include "dataset.h"

Dataset::Dataset()
{
    xpoint.resize(0);
    ypoint.resize(0);
}
Dataset::Dataset(Problem *p,int N)
{
    xpoint.resize(N);
    ypoint.resize(N);
    for(int i=0;i<N;i++)
    {
        xpoint[i].resize(p->getDimension());
        xpoint[i]=p->getSample();
        ypoint[i]=p->statFunmin(xpoint[i]);
    }
}

void Dataset::addPoint(Data &x,double y)
{
    xpoint.push_back(x);
    ypoint.push_back(y);
}
//arff
Dataset::Dataset(QString filename)
{
    FILE *fp=fopen(filename.toStdString().c_str(),"r");
    if(!fp) return;
    int d,count;
    fscanf(fp,"%d",&d);
    fscanf(fp,"%d",&count);
    ypoint.resize(count);

    xpoint.resize(count);
    for(int i=0;i<count;i++)
    {
        xpoint[i].resize(d);
        for(int j=0;j<d;j++)
            fscanf(fp,"%lf",&xpoint[i][j]);
        fscanf(fp,"%lf",&ypoint[i]);
    }
    fclose(fp);
    makePatternClass();
}

/** euresi monadikon katigorion **/
void    Dataset::makePatternClass()
{


    patternClass.resize(0);
    for(int i=0;i<ypoint.size();i++)
    {
        bool found=false;
        for(int j=0;j<patternClass.size();j++)
        {
            //epeidi oi dekadikoi den anaparistantai panta akribos
            if(fabs(patternClass[j]-ypoint[i])<1e-5)
            {
                found=true;
                break;
            }
        }
        if(!found) patternClass.push_back(ypoint[i]);
    }
}

void    Dataset::setData(Matrix &x,Data &y)
{
    if(x.size()!=y.size()) return;
    xpoint.resize(x.size());
    ypoint.resize(y.size());
    int d=x[0].size();
    for(int i=0;i<x.size();i++)
    {
        xpoint[i].resize(d);
        for(int j=0;j<d;j++)
            xpoint[i][j]=x[i][j];
        ypoint[i]=y[i];
    }
    makePatternClass();
}

void    Dataset::saveData(char *filename)
{
    FILE *fp=fopen(filename,"w");
    if(!fp) return;
    fprintf(fp,"%d\n%d\n",dimension(),count());
    for(int i=0;i<count();i++)
    {
        for(int j=0;j<dimension();j++)
            fprintf(fp,"%lf ",xpoint[i][j]);
        fprintf(fp,"%lf\n",ypoint[i]);
    }
    fclose(fp);
}


//epistrefei ena pattern se mia thesi
Data    Dataset::getXPoint(int pos) const
{
    if(pos<0 || pos>=count()) return xpoint[0];
    return xpoint[pos];
}


//epistrefei exodo
double  Dataset::getYPoint(int pos) const
{
    if(pos<0 || pos>=count()) return -1;
    return ypoint[pos];
}

//pos = > number of feature
double  Dataset::meanx(int pos) const
{
    if(pos<0 || pos>=count()) return -1;
    double s=0.0;
    for(int i=0;i<xpoint.size();i++)
        s+=xpoint[i][pos];
    return s/xpoint.size();
}

double  Dataset::stdx(int pos) const
{
    if(pos<0 || pos>=count()) return -1;
    double s=0.0;
    double mx=meanx(pos);
    for(int i=0;i<xpoint.size();i++)
        s+=(xpoint[i][pos]-mx)*(xpoint[i][pos]-mx);
    return sqrt(1.0/xpoint.size() * s);
}


//mesos oros tis exodou
double  Dataset::meany() const
{
    double s=0.0;
    for(int i=0;i<ypoint.size();i++)
        s+=ypoint[i];
    return s/ypoint.size();
}

double  Dataset::stdy() const
{
    double my=meany();
    double s=0.0;
    for(int i=0;i<ypoint.size();i++)
        s+=(ypoint[i]-my)*(ypoint[i]-my);
    return sqrt(1.0/ypoint.size() * s);
}


//=> pos number of feature
double  Dataset::maxx(int pos) const
{
    double m=xpoint[0][pos];
    for(int i=0;i<xpoint.size();i++)
        if(xpoint[i][pos]>m) m=xpoint[i][pos];
    return m;
}

double  Dataset::minx(int pos) const
{
    double m=xpoint[0][pos];
    for(int i=0;i<xpoint.size();i++)
        if(xpoint[i][pos]<m) m=xpoint[i][pos];
    return m;
}

double  Dataset::miny() const
{
    double m=ypoint[0];
    for(int i=0;i<ypoint.size();i++)
        if(ypoint[i]<m) m=ypoint[i];
    return m;
}

double  Dataset::maxy() const
{
    double m=ypoint[0];
    for(int i=0;i<ypoint.size();i++)
        if(ypoint[i]>m) m=ypoint[i];
    return m;
}

int     Dataset::count() const
{
    return ypoint.size();
}


//epistrefei stiles
int     Dataset::dimension() const
{
    if(xpoint.size()==0)
        return 0;
    return xpoint[0].size();
}


Data    Dataset::getPatternClass()
{
    return patternClass;
}

void    Dataset::normalizeMinMax()
{
    Data MinX,MaxX;
    double MinY,MaxY;

    //ypologizo megisto elaxisto ana feature
    MinX.resize(dimension());
    MaxX.resize(dimension());
    for(int i=0;i<dimension();i++)
    {
        MinX[i]=minx(i);
        MaxX[i]=maxx(i);
    }
    MinY=miny();
    MaxY=maxy();
    for(int i=0;i<count();i++)
    {
        for(int j=0;j<dimension();j++)
        {
            xpoint[i][j]=(MaxX[j]-xpoint[i][j])/(MaxX[j]-MinX[j]);
        }
        ypoint[i]=(MaxY-ypoint[i])/(MaxY-MinY);
    }
}

double  Dataset::getClass(double value) const
{
    int imin =-1;
    double dmin=1e+100;
    for(int i=0;i<patternClass.size();i++)
        if(fabs(value-patternClass[i])<dmin)
        {
            dmin = fabs(value - patternClass[i]);
            imin = i;
        }

    return patternClass[imin];
}

vector<Data>    Dataset::getAllXpoint()
{
    return xpoint;
}

Data Dataset::getAllYpoint()
{
    return ypoint;
}


void    Dataset::clearPoints()
{
	auto pos1 = xpoint.begin();
	auto last1 = xpoint.end();
	xpoint.erase(pos1,last1);
	auto pos2=ypoint.begin();
	auto end2=ypoint.end();
	ypoint.erase(pos2,end2);
}

double  Dataset::getClass(int pos) const
{
    double y=ypoint[pos];
    return getClass(y);
}

void    Dataset::setPoint(int pos,Data &x,double y)
{

    xpoint[pos]=x;
    ypoint[pos]=y;
}

Dataset::~Dataset()
{
	auto pos1 = xpoint.begin();
	auto last1 = xpoint.end();
	xpoint.erase(pos1,last1);
	auto pos2=ypoint.begin();
	auto end2=ypoint.end();
	ypoint.erase(pos2,end2);
}
