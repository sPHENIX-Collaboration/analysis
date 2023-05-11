#include <iostream>
#include <ostream>
#include <fstream>
#include <cctype>
#include <string>
#include <math.h>
#include <cstdlib>

#include "TROOT.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TNtuple.h"
#include "TFile.h"
#include "TRandom.h"
#include "TMath.h"
#include "TCanvas.h"
 

char title[100];
char cfile[100];
//char *cflag"";

// length, width of the rectangular towers is now in "arbitrary units",
// essentially something about a Moliere-radius

const double dlength = 20.0; // tower length, arb units
const double ddedx = 0.0025;  // deposit in one step of mip
const double ddzmip = 0.2; // that is constant as opposed to had stepsize

// The following parameters are the most crucial to play with
// for instance dmipstop=0.994 with 100 steps gives 55% probability that the particle
// will never start a shower, survives as MIP
const double dmipstop = 0.994; // if uniform ends up above this, stop MIP
const double dresol = 0.18;  // resolution
//const double ddeem = 0.05;  // deposit in one step of em part
//const double ddehad = 0.1;  // deposit in one step of hadronic
// Width terms of the em and hadronic showers, 
// irrelevant for total deposit (modulo edge effects), crucial for shape
const double dwidthem = 0.11;
const double dwidthhad = 0.3;

const double dtothade = 12.0;    // May want to make a runtime param


const int ntry=3000;  // generate that many particles
const int ndefmipstep = 100;  // max step of MIP
const int ndefshstep = 1000;  // default number of steps for hadron showers
const int nlive = 100;  // 1/frequency of live display


const int nbin=40;
const double xlo=-20.0;
const double xhi=20.0;
const double ylo=-20.0;
const double yhi=20.0;
const double zlo=0.0;
const double zhi=20.0;
