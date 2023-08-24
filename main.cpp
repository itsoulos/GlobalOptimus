#include <QCoreApplication>
# include <PROBLEMS/rastriginproblem.h>
# include <METHODS/gradientdescent.h>
# include <METHODS/multistart.h>
int main(int argc, char *argv[])
{
    srand(2);
    RastriginProblem p;
    Data x = p.getSample();
    double y = p.funmin(x);
    printf(" y = %lf \n",y);
    Multistart gd;
    QStringList lista = gd.getParameterNames();
    qDebug()<<"Lista "<<lista<<endl;
    gd.setProblem(&p);
    gd.setParam("opt_debug","yes");
    gd.setParam("gd_linesearch","golden");

    gd.solve();
    return 0;
}
