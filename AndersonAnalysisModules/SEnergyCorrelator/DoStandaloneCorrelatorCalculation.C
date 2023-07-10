// ----------------------------------------------------------------------------
// 'DoStandaloneCorrelatorCalculation.C'
// Derek Anderson
// 02.02.2023
//
// Use this to run the SEnergyCorrelator
// class in standalone mode.
// ----------------------------------------------------------------------------

#ifndef DOSTANDALONECORRELATORCALCULATION_C
#define DOSTANDALONECORRELATORCALCULATION_C

// standard c includes
#include <string>
#include <vector>
#include <cstdlib>
#include <utility>
// user includes
#include "/sphenix/user/danderson/install/include/senergycorrelator/SEnergyCorrelator.h"

// load libraries
R__LOAD_LIBRARY(/sphenix/user/danderson/install/lib/libsenergycorrelator.so)

using namespace std;

// global constants
static const size_t NHistRange   = 2;
static const size_t NAcceptRange = 2;
static const size_t NEnergyCorrs = 2;



void DoStandaloneCorrelatorCalculation() {

  // io parameters
  const string inFile("/sphenix/user/danderson/eec/SCorrelatorJetTree/output/condor/final_merge/correlatorJetTree.pp200py8jet30run6_trksAndChrgPars.d24m2y2023.root");
  const string inTree[NEnergyCorrs]  = {"RecoJetTree",
                                        "TruthJetTree"};
  const string outFile[NEnergyCorrs] = {"pp200run6jet40.forErrorTest_getHistVars_reco.d16m4y2023.root",
                                        "pp200run6jet40.forErrorTest_getHistVars_true.d16m4y2023.root"};

  // correlator parameters
  const uint32_t  nPointCorr             = 2;
  const uint64_t  nBinsDr                = 75;
  const double    binRangeDr[NHistRange] = {1e-5, 1.};
  const bool      isTruth[NEnergyCorrs]  = {false, true};

  // jet/cst parameters
  const double etaJetRange[NAcceptRange] = {-1., 1.};
  const double momCstRange[NAcceptRange] = {0.,  100.};
  const double drCstRange[NAcceptRange]  = {0.,  5.};

  // jet pT bins
  const vector<pair<double, double>> ptJetBins = {{5., 10.}, {10., 15.}, {15., 20.}, {20., 30.}, {30., 50.}};

  // misc parameters
  const int  verbosity = 0;
  const bool isComplex = false;
  const bool doDebug   = false;
  const bool inBatch   = false;

  // do correlator calculation on reco jets
  SEnergyCorrelator *recoCorrelator = new SEnergyCorrelator("SRecoEnergyCorrelator", isComplex, doDebug, inBatch);
  recoCorrelator -> SetVerbosity(verbosity);
  recoCorrelator -> SetInputFile(inFile);
  recoCorrelator -> SetInputTree(inTree[0], isTruth[0]);
  recoCorrelator -> SetOutputFile(outFile[0]);
  recoCorrelator -> SetJetParameters(ptJetBins, etaJetRange[0], etaJetRange[1]);
  recoCorrelator -> SetConstituentParameters(momCstRange[0], momCstRange[1], drCstRange[0], drCstRange[1]);
  recoCorrelator -> SetCorrelatorParameters(nPointCorr, nBinsDr, binRangeDr[0], binRangeDr[1]);
  recoCorrelator -> Init();
  recoCorrelator -> Analyze();
  recoCorrelator -> End();

  // do correlator calculation on truth jets
  SEnergyCorrelator *trueCorrelator = new SEnergyCorrelator("STrueEnergyCorrelator", isComplex, doDebug, inBatch);
  trueCorrelator -> SetVerbosity(verbosity);
  trueCorrelator -> SetInputFile(inFile);
  trueCorrelator -> SetInputTree(inTree[1], isTruth[1]);
  trueCorrelator -> SetOutputFile(outFile[1]);
  trueCorrelator -> SetJetParameters(ptJetBins, etaJetRange[0], etaJetRange[1]);
  trueCorrelator -> SetConstituentParameters(momCstRange[0], momCstRange[1], drCstRange[0], drCstRange[1]);
  trueCorrelator -> SetCorrelatorParameters(nPointCorr, nBinsDr, binRangeDr[0], binRangeDr[1]);
  trueCorrelator -> Init();
  trueCorrelator -> Analyze();
  trueCorrelator -> End();

}

#endif

// end ------------------------------------------------------------------------
