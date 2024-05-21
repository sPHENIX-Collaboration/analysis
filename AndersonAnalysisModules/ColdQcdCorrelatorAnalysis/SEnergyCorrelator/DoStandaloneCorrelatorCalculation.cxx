// ----------------------------------------------------------------------------
// 'DoStandaloneCorrelatorCalculation.cxx'
// Derek Anderson
// 02.02.2023
//
// Use this to run the SEnergyCorrelator class in standalone mode.
// ----------------------------------------------------------------------------

#ifndef DOSTANDALONECORRELATORCALCULATION_CXX
#define DOSTANDALONECORRELATORCALCULATION_CXX

// c++ utilities
#include <string>
#include <vector>
#include <cstdlib>
#include <utility>
// analysis utilities
#include "/sphenix/user/danderson/install/include/senergycorrelator/SEnergyCorrelator.h"
#include "/sphenix/user/danderson/install/include/senergycorrelator/SEnergyCorrelatorConfig.h"
#include "EnergyCorrelatorOptions.h"

using namespace std;
using namespace SColdQcdCorrelatorAnalysis;

// load libraries
R__LOAD_LIBRARY(/sphenix/user/danderson/install/lib/libsenergycorrelator.so)
R__LOAD_LIBRARY(/sphenix/user/danderson/install/lib/libscorrelatorutilities.so)



// macro body -----------------------------------------------------------------

void DoStandaloneCorrelatorCalculation(const bool doBatch = false, const int verbosity = 0) {

  // input/output files
  const vector<string> inFiles = {
    "../SCorrelatorJetTreeMaker/output/condor/final_merge/correlatorJetTree.pp200py8jet10run8_trksWithRoughCuts.d26m9y2023.root"
  };
  const pair<string, string> outFiles = {
    "twoPoint.pp200py8jet10run8.refactor_cleanUpCutFunctions_reco.d19m3y2024.root",
    "twoPoint.pp200py8jet10run8.refactor_cleanUpCutFunctions_true.d19m3y2024.root"
  };

  // get module configurations
  SEnergyCorrelatorConfig cfg_reco = EnergyCorrelatorOptions::GetRecoConfig(inFiles, outFiles.first, doBatch, verbosity);
  SEnergyCorrelatorConfig cfg_true = EnergyCorrelatorOptions::GetTruthConfig(inFiles, outFiles.second, doBatch, verbosity);

  // do correlator calculation on reco jets
  SEnergyCorrelator* recoCorrelator = new SEnergyCorrelator(cfg_reco);
  recoCorrelator -> Init();
  recoCorrelator -> Analyze();
  recoCorrelator -> End();

  // do correlator calculation on truth jets
  SEnergyCorrelator* trueCorrelator = new SEnergyCorrelator(cfg_true);
  trueCorrelator -> Init();
  trueCorrelator -> Analyze();
  trueCorrelator -> End();
  return;

}

#endif

// end ------------------------------------------------------------------------
