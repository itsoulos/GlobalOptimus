#include <METHODS/usermethod.h>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <iomanip>

/** ylopoiisi tis Pure Random Search **/
UserMethod::UserMethod()
{
    /** proti param einai posa deigmata
     *  tha parei se kathe  iteration
     *  kai deuteri param poses epanalipseis na kanei **/
    addParam(
        Parameter("prs_samples","10","Number of samples")
        );
    addParam(
        Parameter("prs_maxiters","20","Maximum number of iters")
        );
}

UserMethod::~UserMethod()
{
}

void UserMethod::init()
{
    ybest = 10000000;
    /** arxikopoihsh parametron
     *  opos mas tis dinei to optimus **/
    iter = 0;
    maxiters = getParam("prs_maxiters").getValue().toInt();
    samples = getParam("prs_samples").getValue().toInt();
}

void UserMethod::step()
{
    for(int i=0;i<samples;i++)
    {
        /** xsample einai ena neo simeio me uniform sampling **/
        Data xsample = myProblem->getSample();
        double ysample = myProblem->statFunmin(xsample);
        /** edo koitazo an to ysample <  ybest **/
        if(ysample<ybest)
        {
            ybest = ysample;
            xbest = xsample;
        }
    }
}

bool UserMethod::terminated()
{
    /** edo katagrafo posa iters ekana kai an einai
     *  pio pola apo maxiters stamatao **/
    iter++;
    if(iter>=maxiters) return true;
    else return false;
}

void UserMethod::done()
{
    /** trexo to localSearch gia na brei
     *  pragmatiko local minimum **/
    ybest = localSearch(xbest);
}
