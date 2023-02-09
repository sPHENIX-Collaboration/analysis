// 'DoStandaloneCorrelatorCalculation.C'
// Derek Anderson
// 02.02.2023
//
// Use this to run the SEnergyCorrelator
// class in standalone mode.

#ifndef DOSTANDALONECORRELATORCALCULATION_C
#define DOSTANDALONECORRELATORCALCULATION_C

// standard c includes
#include <string>
#include <vector>
#include <cstdlib>
#include <utility>
// user includes
#include "/sphenix/user/danderson/install/include/senergycorrelator/SEnergyCorrelator.h"

#ifdef __CINT__ 
#pragma link C++ nestedclasses;
#pragma link C++ nestedtypedefs;
#pragma link C++ class std::vector<std::vector<double> >+;
#pragma link C++ class std::vector<std::pair<double,double> >+;
#endif

// load libraries
R__LOAD_LIBRARY(/sphenix/user/danderson/install/lib/libsenergycorrelator.so)

using namespace std;

// global constants
static const size_t NHistRange   = 2;
static const size_t NEnergyCorrs = 2;



void DoStandaloneCorrelatorCalculation() {

  // io parameters
  const string inFile("/sphenix/user/danderson/eec/SCorrelatorJetTree/output/condor/final_merge/correlatorJetTree.pp200py8run50_tracks_withQAHists.d2m2y2023.root");
  const string inReco("RecoJetTree");
  const string inTrue("TruthJetTree");
  const string outFile("test.root");

  // correlator parameters
  const uint32_t  nPointCorr             = 2;
  const uint64_t  nBinsDr                = 75;
  const double    binRangeDr[NHistRange] = {1e-5, 1.};
  const bool      isTruth[NEnergyCorrs]  = {false, true};

  // pTjet bins
  const vector<pair<double, double>> ptJetBins = {{5., 10.}, {10., 15.}, {15., 20.}, {20., 30.}, {30., 50.}};

  // misc parameters
  const int  verbosity = 0;
  const bool isComplex = false;
  const bool doDebug   = true;

  // do correlator calculation on reco jets
  SEnergyCorrelator *recoCorrelator = new SEnergyCorrelator("SRecoEnergyCorrelator", isComplex, doDebug);
  recoCorrelator -> SetVerbosity(verbosity);
  recoCorrelator -> SetInputFile(inFile);
  recoCorrelator -> SetInputTree(inReco, isTruth[0]);
  recoCorrelator -> SetOutputFile(outFile);
  recoCorrelator -> SetCorrelatorParameters(nPointCorr, nBinsDr, binRangeDr[0], binRangeDr[1]);
  recoCorrelator -> SetPtJetBins(ptJetBins);
  recoCorrelator -> Init();
  recoCorrelator -> Analyze();
  recoCorrelator -> End();

  // do correlator calculation on truth jets
  SEnergyCorrelator *trueCorrelator = new SEnergyCorrelator("STrueEnergyCorrelator", isComplex, doDebug);
  trueCorrelator -> SetVerbosity(verbosity);
  trueCorrelator -> SetInputFile(inFile);
  trueCorrelator -> SetInputTree(inTrue, isTruth[1]);
  trueCorrelator -> SetOutputFile(outFile);
  recoCorrelator -> SetCorrelatorParameters(nPointCorr, nBinsDr, binRangeDr[0], binRangeDr[1]);
  recoCorrelator -> SetPtJetBins(ptJetBins);
  trueCorrelator -> Init();
  trueCorrelator -> Analyze();
  trueCorrelator -> End();

}

#endif

// end ------------------------------------------------------------------------
