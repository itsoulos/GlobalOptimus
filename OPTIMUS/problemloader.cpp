#include "problemloader.h"
ProblemLoader::ProblemLoader()
{
    myProblem = NULL;
    problemList<<"rastrigin"<<"test2n"<<"bf1"<<"bf2"
              <<"equal_maxima"<<"f1"<<"f3"<<"f5"<<"f9"
             <<"himmelblau"<<"shubert"<<"vincent"
        <<"branin"<<"camel"<<"cm"<<"diffpower"<<"easom"<<"exp"
        <<"elp"<<"gkls250"<<"gkls2100"<<"gkls350"
                <<"bf3"<<"extendedf10"
                <<"goldstein"<<"griewank2"<<"griewank10"<<"schwefel221"
                <<"hansen"<<"hartman3"<<"hartman6"<<"potential"<<"schwefel"
                <<"sphere"<<"schwefel222"<<"f16"<<"f17"<<"f18"<<"f19"
                <<"f12"<<"f13"<<"f14"<<"f15"<<"schaffer"
                <<"rosenbrock"<<"shekel5"<<"shekel7"<<"shekel10"<<"sckley"
                <<"sinu"<<"test30n"<<"mlpproblem"<<"rbfproblem"
		<<"brenner"<<"userproblem";
    myParams["dummy"]=1;
    selectedProblem = "";
}

QString         ProblemLoader::getSelectedProblemName() const
{
    return selectedProblem;
}

QStringList     ProblemLoader::getProblemList() const
{
    return problemList;
}

QString ProblemLoader::getProblemReport(QString selectedproblem,
                                        Problem *myProblem)
{
    if(myProblem == NULL) return "";
    QString ret ="\nProblem Name: "+selectedproblem+"\n";
    //random sample
    Data x = myProblem->getBestx();
    if(x.size()==0)
        x=myProblem->getSample();
    double fx = myProblem->statFunmin(x);
    QString header="============================================";
    ret+="Current Point.\n"+header+"\n";
    for(int i=0;i<(int)x.size();i++)
    {
        ret+="x["+QString::asprintf("%d",i)+"]="+
                QString::asprintf("%10.5lf",x[i])+"\n";
    }
    ret+="F(X)="+QString::asprintf("%10.5lf\n",fx);
    ret+="\nFunction Margins\n"+header+"\n";
    Data xl = myProblem->getLeftMargin();
    Data xr = myProblem->getRightMargin();
    for(int i=0;i<(int)xl.size();i++)
    {
        ret+="Left["+QString::asprintf("%d",i)+"]="+
                QString::asprintf("%8.4lf",xl[i])+
                " Right["+QString::asprintf("%d",i)+"]="+
                QString::asprintf("%8.4lf",xr[i])+"\n";
    }
    Data g = myProblem->gradient(x);
    ret+="\nGradient info\n"+header+"\n";
    for(int i=0;i<(int)g.size();i++)
    {
        ret+="Grad["+QString::asprintf("%d",i)+"]="+
                QString::asprintf("%10.5lf",g[i])+"\n";
    }
    ret+="GRMS(X)="+QString::asprintf("%10.5lf",
                                      myProblem->grms(x))+"\n";

    return ret;
}

Problem *ProblemLoader::getProblemFromName(QString name)
{
    Problem *myProblem=NULL;
    Data xx;
    double yy;
    if(name == "rastrigin")
    {
        myProblem = new RastriginProblem();
        xx.resize(2);
        xx[0]=0;
        xx[1]=1;
        yy= -2.0;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "ackley")
    {
        myProblem = new Ackley();
        xx.resize(2);
        xx[0]=0;
        xx[1]=0;
        yy= 0.0;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "bf3")
    {

        myProblem = new bf3();
        xx.resize(2);
        xx[0]=0;
        xx[1]=0;
        yy= 0.0;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "extendedf10")
    {

        myProblem = new Extended_f10();
        xx.resize(2);
        xx[0]=0;
        xx[1]=0;
        yy= 0.0;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "schwefel")
    {

        myProblem = new schwefel();
    }
    else
    if(name == "schwefel221")
    {
       myProblem = new schwefel221();
    }
    else
    if(name == "schwefel222")
        myProblem = new schwefel222();
    else
    if(name  == "schaffer")
    {

        myProblem = new schaffer();
    }
    else
    if(name == "f16")
        myProblem = new f16();
    else
    if(name == "f17")
        myProblem = new f17();
    else
    if(name == "f18")
        myProblem = new f18();
    else
    if(name == "f19")
        myProblem = new f19();
    else
    if(name == "f12")
        myProblem = new f12();
    else
    if(name == "f13")
        myProblem = new f13();
    else
    if(name == "f14")
        myProblem = new f14();
    else
    if(name == "f15")
        myProblem = new f15();
    else
    if(name =="sphere")
        myProblem = new Sphere();
    else
    if(name == "test2n")
        myProblem = new Test2nProblem();
    else
    if(name == "bf1")
    {

        myProblem = new Bf1();
        xx.resize(2);
        xx[0]=0;
        xx[1]=0;
        yy= 0.0;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "bf2")
    {
        myProblem = new Bf2();
        xx.resize(2);
        xx[0]=0;
        xx[1]=0;
        yy= 0.0;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "branin")
    {
        myProblem = new Branin();
        xx.resize(2);
        xx[0]=-3.141593;
        xx[1]=5.683451;
        yy= 0.397887;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "camel")
    {
        myProblem = new Camel();
        xx.resize(2);
        xx[0]=-0.089842;
        xx[1]=0.712656;
        yy= -1.031628 ;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "cm")
        myProblem = new Cm();
    else
    if(name == "diffpower")
        myProblem = new DiffPower();
    else
    if(name == "easom")
    {
        myProblem = new Easom();
        xx.resize(2);
        xx[0]=-87.964594;
        xx[1]= 85.636858;
        yy= -1.0;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "exp")
        myProblem = new Exp();
    else
    if(name == "elp")
        myProblem = new Elp();
    else
    if(name == "gkls250")
    {
        myProblem = new Gkls250();
        xx.resize(2);
        xx[0]=0.225168;
        xx[1]=0.283428;
        yy= -1.0;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "gkls2100")
    {
        myProblem = new Gkls2100();
        xx.resize(2);
        xx[0]=0.315437;
        xx[1]=0.374783;
        yy= -1.0;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "gkls350")
    {
        myProblem = new Gkls350();
        xx.resize(3);
        xx[0]=0.006579;
        xx[1]= -0.291979;
        xx[2]=0.082807;
        yy= -1.0;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "goldstein")
    {
        myProblem = new Goldstein();
        xx.resize(2);
        xx[0]=0;
        xx[1]=-1.0;
        yy=3.0;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "griewank2")
    {
        myProblem = new Griewank2();
        xx.resize(2);
        xx[0]=0;
        xx[1]=0;
        yy=0;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "griewank10")
    {
        myProblem = new Griewank10();
        xx.resize(10);
        for(int i=0;i<10;i++) xx[i]=0.0;
        yy=0.0;
        myProblem->setKnownOptimum(yy,xx);

    }
    else
    if(name == "hansen")
    {
        myProblem = new Hansen();
        xx.resize(2);
        xx[0]=4.976478;
        xx[1]=-7.708314;
        yy=-176.541793;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "hartman3")
    {
        myProblem = new Hartman3();
        xx.resize(3);
        xx[0]=0.114614;
        xx[1]=0.555649;
        xx[2]=0.852547;
        yy=-3.862782;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "hartman6")
    {
        myProblem = new Hartman6();
        xx.resize(6);
        xx[0]=0.201690;
        xx[1]=0.150011;
        xx[2]=0.476874;
        xx[3]=0.275332;
        xx[4]=0.311652;
        xx[5]=0.657301;

        yy=-3.322368;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "potential")
        myProblem = new Potential();
    else
    if(name == "rosenbrock")
        myProblem = new Rosenbrock();
    else
    if(name == "shekel5")
    {
        myProblem = new Shekel5();
        xx.resize(4);
        xx[0]=3.999953 ;
        xx[1]=4.000049;
        xx[2]=3.999953;
        xx[3]=4.000049;
        yy=-10.107749;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "shekel7")
    {
        myProblem = new Shekel7();
        xx.resize(4);
        xx[0]=4.000420;
        xx[1]=4.000537;
        xx[2]=3.999474;
        xx[3]=3.999591;
        yy=-10.342378;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "shekel10")
    {
        myProblem = new Shekel10();
        xx.resize(4);
        xx[0]=4.000747 ;
        xx[1]=4.000593;
        xx[2]=3.999663;
        xx[3]= 3.999510 ;
        yy=-10.536410;
        myProblem->setKnownOptimum(yy,xx);
    }
    else
    if(name == "sinu")
        myProblem = new Sinu();
    else
    if(name=="test2n")
        myProblem = new Test2nProblem();
    if(name == "test30n")
        myProblem = new Test30n();
    else
    if(name == "mlpproblem")
        myProblem = new MlpProblem();
    else
    if(name == "rbfproblem")
        myProblem = new RbfProblem();
    else
    if(name == "gross")
    myProblem = new Gross();
    else
    if(name == "equal_maxima")
        myProblem =new Equal_Maxima();
    else
    if(name == "f1")
        myProblem = new F1();
    else
    if(name == "f3")
        myProblem = new f3();
    else
    if(name == "f5")
        myProblem = new f5();
    else
    if(name == "f9")
        myProblem = new f9();
    else
    if(name == "himmelblau")
        myProblem = new Himmelblau();
    else
    if(name == "shubert")
        myProblem = new shubert();
    else
    if(name == "vincent")
        myProblem = new Vincent();
    else 
    if(name == "brenner")
	myProblem = new E_ev();
    else    
    if(name == "userproblem")
        myProblem = new UserProblem();
    return myProblem;

}

Problem         *ProblemLoader::loadProblem(QString name)
{
    selectedProblem="";
    if(myProblem!=NULL) delete myProblem;
    if(problemList.contains(name)==false)
        return NULL;
    selectedProblem = name;
    myProblem = ProblemLoader::getProblemFromName(name);
    return myProblem;
}

void            ProblemLoader::setParams(QJsonObject &p)
{
    myParams= p;
    if(myProblem!=NULL &&
            selectedProblem!="mlpproblem"
            && selectedProblem!="rbfproblem")
        myProblem->init(myParams);
}

QJsonObject     ProblemLoader::getParams() const
{
    return myParams;
}

ProblemLoader::~ProblemLoader()
{
    if(myProblem!=NULL)
        delete myProblem;
}
