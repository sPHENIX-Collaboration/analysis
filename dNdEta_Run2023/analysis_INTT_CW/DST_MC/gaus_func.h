#ifndef GausFunc_h
#define GausFunc_h

#include <TMath.h>

double gaus_func(double *x, double *par)
{
    // note : par[0] : size
    // note : par[1] : mean
    // note : par[2] : width
    // note : par[3] : offset 
    return par[0] * TMath::Gaus(x[0],par[1],par[2]) + par[3];
}

double gaus_pol1_func(double *x, double *par)
{
    // note : par[0] : size
    // note : par[1] : mean
    // note : par[2] : width
    // note : par[3] : offset 
    return par[0] * TMath::Gaus(x[0],par[1],par[2]) + par[3] + par[4]*x[0];
}

double gaus_pol2_func(double *x, double *par)
{
    // note : par[0] : size
    // note : par[1] : mean
    // note : par[2] : width

    double gaus_func = par[0] * TMath::Gaus(x[0],par[1],par[2]);
    double pol2_func = par[3] + par[4]* (x[0]-par[6]) + par[5] * pow((x[0]-par[6]),2);

    return gaus_func + pol2_func;
}



double d_gaus_pol1_func(double *x, double *par)
{

    // note : par[0] : size
    // note : par[1] : ratio of the two gaussians
    // note : par[2] : mean
    // note : par[3] : width of gaus 1
    // note : par[4] : width of gaus 2
    // note : par[5] : offset
    // note : par[6] : slope
    return par[0] * ( (1. - par[1]) * TMath::Gaus(x[0],par[2],par[3]) + par[1] * TMath::Gaus(x[0],par[2],par[4]) ) + par[5] + par[6] * x[0];
}

double d_gaus_func(double *x, double *par)
{

    // note : par[0] : size
    // note : par[1] : ratio of the two gaussians
    // note : par[2] : mean
    // note : par[3] : width of gaus 1
    // note : par[4] : width of gaus 2
    return par[0] * ( (1. - par[1]) * TMath::Gaus(x[0],par[2],par[3]) + par[1] * TMath::Gaus(x[0],par[2],par[4]) );
}

double bkg_pol2_func(double *x, double *par)
{
    if (x[0] > (-1 * par[4]) && x[0] < par[4]) {
      TF1::RejectPoint();
      return 0;
   }
   return par[0] + par[1]* (x[0]-par[3]) + par[2] * pow((x[0]-par[3]),2);

   // note : p[0] + p[1]*(x-p[3])+p[2] * (x-p[3])^2, p[4] sets the signal range that should be excluded in the fit
}

double full_pol2_func(double *x, double *par)
{
   return par[0] + par[1]* (x[0]-par[3]) + par[2] * pow((x[0]-par[3]),2);

   // note : p[0] + p[1]*(x-p[3])+p[2] * (x-p[3])^2
}





#endif