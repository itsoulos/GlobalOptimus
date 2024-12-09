#ifndef E_EV_H
#define E_EV_H
#include "OPTIMUS/problem.h"

class E_ev : public Problem
{
public:
E_ev();
double funmin(Data &x);
Data gradient(Data &x) ;
double Ni(const Data &x, size_t i);
double Nj(const Data &x, size_t j);
double calcNeighbor(const Data &x, size_t index);
double F(const Data &x, size_t  i, size_t k);
double NijConj(const Data &x, size_t i, size_t j);
double calculateAngle(const Data &x, size_t i, size_t j, size_t k);
double Gc(double theta);
double Bij(const Data &x, size_t i, size_t j);
double distance(const Data &x, size_t i, size_t j);
double fij(double rij);
double VR(double rij) ;
double Fij(const Data &x, size_t i, size_t j);
void init(QJsonObject &params);
double VA(double rij);
};
#endif // E_EV_H


