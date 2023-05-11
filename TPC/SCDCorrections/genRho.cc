#include <iostream>
#include <fstream>
#include <string>
#include "QPileUpToy.h"

int main() {
  std::string kFileNameRoot="rho";
  float kInnerRadius = 30; //cm
  float kOutterRadius = 80; //cm
  float kHalfLength = 80; //cm
  int kNRadialSteps;
  int kNAzimuthalSteps;
  int kNLongitudinalSteps;

  std::cout << "fetching geo.dat..." << std::endl;
  std::ifstream ifile("geo.dat");
  ifile >> kFileNameRoot;
  ifile >> kInnerRadius;  //cm
  ifile >> kOutterRadius; //cm
  ifile >> kHalfLength;   //cm
  ifile.close();
  kNRadialSteps = (kOutterRadius-kInnerRadius)*10;
  kNAzimuthalSteps = 18;
  kNLongitudinalSteps = kHalfLength*20;


  QPileUp *initialdensity;
  initialdensity = new QPileUpToy(1.0/76628.0/*gasfactor [Vs]*/, 425.0 /*multiplicity*/, 5e+4/*rate [Hz]*/, 6); // PHENIX
  //initialdensity = new QPileUpToy(1.0/76628.0/*gasfactor [Vs]*/, 950.0 /*multiplicity*/, 5e+4/*rate [Hz]*/, 20, 1.5); // ALICE
  //initialdensity = new QPileUpToy(1.0/76628.0/*gasfactor [Vs]*/, 425.0 /*multiplicity*/, 15e+3/*rate [Hz]*/, 0); // STAR
  initialdensity->SetDebugLevel(2);
  initialdensity->OutputFileName(kFileNameRoot);
  initialdensity->TPCDimensions( kInnerRadius, kOutterRadius, kHalfLength );
  initialdensity->TPCGridSize( kNRadialSteps, kNAzimuthalSteps, kNLongitudinalSteps );
  initialdensity->Make();

  delete initialdensity;
  return 0;
}
