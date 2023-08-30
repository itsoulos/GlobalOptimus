#include "statistics.h"
# include <QDebug>
Statistics::Statistics()
{
    bestyValue.resize(0);
    fcallsValue.resize(0);
}

void Statistics::addProblem(Problem *p)
{
    bestyValue.push_back(p->getBesty());
    fcallsValue.push_back(p->getFunctionCalls());
    //call done
    Data xv = p->getBestx();
    QJsonObject xx =p->done(xv);
    if(xx.isEmpty()) return;
    qDebug()<<"Stats. OBJECT = "<<xx<<endl;
    QStringList list = xx.keys();
    for(int i=0;i<list.size();i++)
    {
        Metric mt;
        mt.name = list[i];
        mt.value = xx[mt.name].toDouble();
        functionMetric.push_back(mt);
     }
}

void Statistics::printStatistics()
{
    int n = (int)bestyValue.size();
    printf("Executions: %4d\n===================\n",n);
    int totalFcalls = 0;
    double miny = 1e+100;
    int timesMinimumFound = 0;
    for(int i=0;i<n;i++)
    {
        printf("RUN: %4d BEST VALUE: %20.10lg FUNCTION CALLS: %4d\n",
               i+1,bestyValue[i],fcallsValue[i]);
        totalFcalls+=fcallsValue[i];
        if(i==0 || bestyValue[i]<miny) miny=bestyValue[i];
    }
    for(int i=0;i<n;i++)
    {
        if(fabs(bestyValue[i]-miny)<1e-6)
            timesMinimumFound++;
    }

    printf("Average Function calls: %20.2lf\n",totalFcalls*1.0/n);
    printf("Minimum Function Value: %20.10lg\n",miny);
    printf("Percent Minimum  Found: %20.2lf%%\n",timesMinimumFound*100.0/n);

    QStringList uniqueNames;
    for(int i=0;i<functionMetric.size();i++)
    {
        if(!uniqueNames.contains(functionMetric[i].name))
            uniqueNames<<functionMetric[i].name;
    }
    for(int i=0;i<uniqueNames.size();i++)
    {
        double minValue = 1e+100;
        double avgValue =  0.0;
        int    times    = 0;
        QString name = uniqueNames[i];
        for(int j=0;j<functionMetric.size();j++)
        {
            if(functionMetric[j].name == name)
            {
                double dv = functionMetric[j].value;
                if(dv<minValue) minValue = dv;
                avgValue+=dv;
                times++;
            }
        }
        printf("METRIC: %10s MIN VALUE: %20.10lg AVG VALUE: %20.10lg\n",
               name.toStdString().c_str(),
               minValue,avgValue/times);
    }
}

Statistics::~Statistics()
{
    //nothing here
}
