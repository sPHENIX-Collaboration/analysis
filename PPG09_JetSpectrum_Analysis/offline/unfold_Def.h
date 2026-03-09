#include <iostream>
#include <TFile.h>
#include <TF1.h>

double calibptbins[] = {15, 19, 24, 29, 35, 41, 48, 56, 65, 75};
double truthptbins[] = {7, 11, 15, 19, 24, 29, 35, 41, 48, 56, 65, 75, 86, 1000};

int n_underflowbin = 2;
int n_overflowbin = 2;

int calibnpt = sizeof(calibptbins) / sizeof(calibptbins[0]) - 1;
int truthnpt = sizeof(truthptbins) / sizeof(truthptbins[0]) - 1;