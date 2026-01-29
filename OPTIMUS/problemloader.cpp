#include "problemloader.h"
ProblemLoader::ProblemLoader()
{
    myProblem = NULL;
    problemList <<"rastrigin2"<< "test2n"<<
                   "bf1"<<"bf2"<<"bf3"<<"branin" << "brenner" <<
                   "bucherastrigin"<<"camel" << "cm" << "diffpower" <<"differentpowers"
                << "discus"<< "diracproblem"<<"easom" << "exp"<< "equalmaxima"
                << "elp" << "gkls250" << "gkls2100" << "gkls350"<< "extendedf10"
                << "gallagher101"<< "gallagher21"<<"ellipsoidal"
                << "goldstein" << "griewank2" << "griewank10"
                << "griewankrosenbrock"<<"griewank"
                << "hansen" << "hartman3" << "hartman6"
        <<"problem1d"
                << "himmelblau"<<"katsuura"<<"lunacekbirastrigin"<<"levy"<<
                    "michalewicz"<<"potential";

    problemList
                << "shubert" << "vincent"
               << "schwefel"
                << "schaffer"<< "rosenbrock" << "shekel5"
                << "shekel7" << "shekel10" << "sckley" <<"rastrigin"
                << "sinu" << "test30n"
                <<"linearslope"<<"attractivesector"<<"stepellipsoidal"
                << "rotatedrosenbrock"<<  "sharpridge"
               <<"weierstrass"
                <<"zakharov"<< "mlpproblem" << "rbfproblem"<<"userproblem";
    myParams["dummy"] = 1;
    selectedProblem = "";
}

QString ProblemLoader::getSelectedProblemName() const
{
    return selectedProblem;
}

QStringList ProblemLoader::getProblemList() const
{
    return problemList;
}

QString ProblemLoader::getProblemReport(QString selectedproblem,
                                        Problem *myProblem)
{
    if (myProblem == NULL)
        return "";
    QString ret = "\nProblem Name: " + selectedproblem + "\n";
    // random sample
    Data x = myProblem->getBestx();
    if (x.size() == 0)
        x = myProblem->getSample();
    double fx = myProblem->statFunmin(x);
    QString header = "============================================";
    ret += "Current Point.\n" + header + "\n";
    for (int i = 0; i < (int)x.size(); i++)
    {
        ret += "x[" + QString::asprintf("%d", i) + "]=" +
               QString::asprintf("%10.5lf", x[i]) + "\n";
    }
    ret += "F(X)=" + QString::asprintf("%10.5lf\n", fx);
    ret += "\nFunction Margins\n" + header + "\n";
    Data xl = myProblem->getLeftMargin();
    Data xr = myProblem->getRightMargin();
    for (int i = 0; i < (int)xl.size(); i++)
    {
        ret += "Left[" + QString::asprintf("%d", i) + "]=" +
               QString::asprintf("%8.4lf", xl[i]) +
               " Right[" + QString::asprintf("%d", i) + "]=" +
               QString::asprintf("%8.4lf", xr[i]) + "\n";
    }

    Data g = myProblem->gradient(x);
    ret += "\nGradient info\n" + header + "\n";
    for (int i = 0; i < (int)g.size(); i++)
    {
        ret += "Grad[" + QString::asprintf("%d", i) + "]=" +
               QString::asprintf("%10.5lf", g[i]) + "\n";
    }
    ret += "GRMS(X)=" + QString::asprintf("%10.5lf", myProblem->grms(x)) + "\n";

    return ret;
}

Problem *ProblemLoader::getProblemFromName(QString name)
{
    Problem *myProblem = NULL;
    if (name == "rastrigin2")
        myProblem = new RastriginProblem();
    else if (name == "ackley")
        myProblem = new Ackley();
    else if (name == "bf3")
        myProblem = new bf3();
    else if (name == "extendedf10")
        myProblem = new Extended_f10();
    else if (name == "schaffer")
        myProblem = new schaffer();
    else if (name == "test2n")
        myProblem = new Test2nProblem();
    else if (name == "bf1")
        myProblem = new Bf1();
    else if (name == "bf2")
        myProblem = new Bf2();
    else if (name == "branin")
        myProblem = new Branin();
    else if (name == "camel")
        myProblem = new Camel();
    else if (name == "cm")
        myProblem = new Cm();
    else if (name == "diffpower")
        myProblem = new DiffPower();
    else if (name == "easom")
        myProblem = new Easom();
    else if (name == "exp")
        myProblem = new Exp();
    else if (name == "elp")
        myProblem = new Elp();
    else if (name == "gkls250")
        myProblem = new Gkls250();
    else if (name == "gkls2100")
        myProblem = new Gkls2100();
    else if (name == "gkls350")
        myProblem = new Gkls350();
    else if (name == "goldstein")
        myProblem = new Goldstein();
    else if (name == "griewank2")
        myProblem = new Griewank2();
    else if (name == "griewank10")
        myProblem = new Griewank10();
    else if (name == "hansen")
        myProblem = new Hansen();
    else if (name == "hartman3")
        myProblem = new Hartman3();
    else if (name == "hartman6")
        myProblem = new Hartman6();
    else if (name == "potential")
        myProblem = new Potential();
    else if (name == "rosenbrock")
        myProblem = new Rosenbrock();
    else if (name == "shekel5")
        myProblem = new Shekel5();
    else if (name == "shekel7")
        myProblem = new Shekel7();
    else if (name == "shekel10")
        myProblem = new Shekel10();
    else if (name == "sinu")
        myProblem = new Sinu();
    else if (name == "test2n")
        myProblem = new Test2nProblem();
    if (name == "test30n")
        myProblem = new Test30n();
    else if (name == "mlpproblem")
        myProblem = new MlpProblem();
    else if (name == "rbfproblem")
        myProblem = new RbfProblem();
    else if (name == "gross")
        myProblem = new Gross();
    else if (name == "equalmaxima")
        myProblem = new EqualMaxima();
    else if (name == "himmelblau")
        myProblem = new Himmelblau();
    else if (name == "shubert")
        myProblem = new shubert();
    else if (name == "vincent")
        myProblem = new Vincent();
    else if (name == "brenner")
        myProblem = new E_ev();
    else if (name == "gallagher101")
        myProblem = new Gallagher101();
    else if (name == "gallagher21")
        myProblem = new Gallagher21();
    else if (name == "ellipsoidal")
        myProblem = new Ellipsoidal();
    else if (name == "rastrigin")
        myProblem = new Rastrigin();
    else if (name == "bucherastrigin")
        myProblem = new BucheRastrigin();
    else if (name == "linearslope")
        myProblem = new LinearSlope();
    else if (name == "attractivesector")
        myProblem = new AttractiveSector();
    else if (name == "stepellipsoidal")
        myProblem = new StepEllipsoidal();
    else if (name == "rotatedrosenbrock")
        myProblem = new RotatedRosenbrock();
    else if (name == "discus")
        myProblem = new Discus();
    else if (name == "sharpridge")
        myProblem = new SharpRidge();
    else if (name == "differentpowers")
        myProblem = new DifferentPowers();
    else if (name == "weierstrass")
        myProblem = new Weierstrass();
    else if (name == "griewankrosenbrock")
        myProblem = new GriewankRosenbrock();
    else if (name == "katsuura")
        myProblem = new Katsuura();
    else if (name == "lunacekbirastrigin")
        myProblem = new LunacekBiRastrigin();
    else if (name == "schwefel")
        myProblem = new Schwefel();
    else if (name == "griewank")
        myProblem = new Griewank();
    else if (name == "diracproblem")
        myProblem = new DiracProblem();
    else if (name == "michalewicz")
        myProblem = new Michalewicz();
    else if (name == "zakharov")
        myProblem = new Zakharov();
    else if (name == "levy")
        myProblem = new Levy();
    else if (name == "problem1d")
        myProblem = new problem1d();
    else if (name == "userproblem")
        myProblem = new UserProblem();
    return myProblem;
}

Problem *ProblemLoader::loadProblem(QString name)
{
    selectedProblem = "";
    if (myProblem != NULL)
        delete myProblem;
    if (problemList.contains(name) == false)
        return NULL;
    selectedProblem = name;
    myProblem = ProblemLoader::getProblemFromName(name);
    return myProblem;
}

void ProblemLoader::setParams(QJsonObject &p)
{
    myParams = p;
    if (myProblem != NULL &&
        selectedProblem != "mlpproblem" && selectedProblem != "rbfproblem")
        myProblem->init(myParams);
}

QJsonObject ProblemLoader::getParams() const
{
    return myParams;
}

ProblemLoader::~ProblemLoader()
{
    if (myProblem != NULL)
        delete myProblem;
}
