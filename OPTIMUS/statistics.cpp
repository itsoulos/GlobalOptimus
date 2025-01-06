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
    QStringList list = xx.keys();
    for(int i=0;i<list.size();i++)
    {
        Metric mt;
        mt.name = list[i];
        mt.value = xx[mt.name].toDouble();
        mt.fvalue= p->getBesty();
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
   double meanFcalls=totalFcalls*1.0/n;
    double sum_deviation_fcalls = 0.0;
           for (int i = 0; i < n; i++)
           {
               sum_deviation_fcalls += pow(fcallsValue[i]-meanFcalls,2);


       }
           double std_deviation_fcalls= sqrt(sum_deviation_fcalls/n);
    printf("Average Function calls: %20.2lf\n",totalFcalls*1.0/n);
    printf("Minimum Function Value: %20.10lg\n",miny);
    printf("Percent Minimum  Found: %20.2lf%%\n",timesMinimumFound*100.0/n);
    printf("Standard Deviation of Function Calls: %20.10lg\n", std_deviation_fcalls);

    QStringList uniqueNames;
    for(int i=0;i<functionMetric.size();i++)
    {
        if(!uniqueNames.contains(functionMetric[i].name))
            uniqueNames<<functionMetric[i].name;
    }
    for(int i=0;i<uniqueNames.size();i++)
    {
        double minValue = 1e+100;
        double maxValue = -1e+100;
        double avgValue =  0.0;
        int    times    = 0;
        QString name = uniqueNames[i];
        for(int j=0;j<functionMetric.size();j++)
        {
            if(functionMetric[j].name == name)
            {
                double dv = functionMetric[j].value;
                if(dv<minValue) minValue = dv;
                if(dv>maxValue) maxValue = dv;
                avgValue+=dv;
                times++;
            }
        }
        printf("METRIC: %10s MIN VALUE: %20.10lg MAX VALUE: %20.10lg AVG VALUE: %20.10lg\n",
               name.toStdString().c_str(),
               minValue,maxValue,avgValue/times);
    }
    //full report

}

QString Statistics::getStatistics()
{
    QString ret="";
    int n = (int)bestyValue.size();
    ret+=QString::asprintf("Executions: %4d\n===================\n",n);
    int totalFcalls = 0;
    double miny = 1e+100;
    int timesMinimumFound = 0;
    for(int i=0;i<n;i++)
    {
        ret+=QString::asprintf("RUN: %4d BEST VALUE: %20.10lg FUNCTION CALLS: %4d\n",
               i+1,bestyValue[i],fcallsValue[i]);
        totalFcalls+=fcallsValue[i];
        if(i==0 || bestyValue[i]<miny) miny=bestyValue[i];
    }
    for(int i=0;i<n;i++)
    {
        if(fabs(bestyValue[i]-miny)<1e-6)
            timesMinimumFound++;
    }

    ret+=QString::asprintf("Average Function calls: %20.2lf\n",totalFcalls*1.0/n);
    ret+=QString::asprintf("Minimum Function Value: %20.10lg\n",miny);
    ret+=QString::asprintf("Percent Minimum  Found: %20.2lf%%\n",timesMinimumFound*100.0/n);

    QStringList uniqueNames;
    for(int i=0;i<functionMetric.size();i++)
    {
        if(!uniqueNames.contains(functionMetric[i].name))
            uniqueNames<<functionMetric[i].name;
    }
    for(int i=0;i<uniqueNames.size();i++)
    {
        double minValue = 1e+100;
        double maxValue = -1e+100;
        double avgValue =  0.0;
        int    times    = 0;
        QString name = uniqueNames[i];
        for(int j=0;j<functionMetric.size();j++)
        {
            if(functionMetric[j].name == name)
            {
                double dv = functionMetric[j].value;
                if(dv<minValue) minValue = dv;
                if(dv>maxValue) maxValue = dv;
                avgValue+=dv;
                times++;
            }
        }
        ret+=QString::asprintf("METRIC: %10s MIN VALUE: %20.10lg MAX VALUE: %20.10lg AVG VALUE: %20.10lg\n",
               name.toStdString().c_str(),
               minValue,maxValue,avgValue/times);
    }
    return ret;
}

Statistics::~Statistics()
{
    //nothing here
}
