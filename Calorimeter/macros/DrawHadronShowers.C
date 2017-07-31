#include <cmath>
#include <TFile.h>
#include <TString.h>
#include <TLine.h>
#include <TTree.h>
#include <cassert>
#include "SaveCanvas.C"
#include "sPhenixStyle.C"
using namespace std;

void
DrawHadronShowers(void)
{
  SetsPhenixStyle();
  TVirtualFitter::SetDefaultFitter("Minuit2");



}


