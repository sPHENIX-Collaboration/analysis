#include "fit_func.h"

double gaus_func(double *x, double *par)
{
    // note : par[0] : size
    // note : par[1] : mean
    // note : par[2] : width
    // note : par[3] : offset 
    return par[0] * TMath::Gaus(x[0],par[1],par[2]) + par[3];
}