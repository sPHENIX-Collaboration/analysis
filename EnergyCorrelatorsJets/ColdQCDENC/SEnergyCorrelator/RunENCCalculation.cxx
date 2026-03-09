/// ---------------------------------------------------------------------------
/*! \file    RunENCCalculation.cxx
 *  \authors Derek Anderson
 *  \date    02.02.2023
 *
 *  A ROOT macro to run the SEnergyCorrelator module.
 */
/// ---------------------------------------------------------------------------

#ifndef RUNENCCALCULATION_CXX
#define RUNENCCALCULATION_CXX

// c++ utilities
#include <string>
#include <vector>
#include <cstdlib>
#include <utility>
// module definition
#include <senergycorrelator/SEnergyCorrelator.h>
// macro options
#include "EnergyCorrelatorOptions.h"

using namespace std;
using namespace SColdQcdCorrelatorAnalysis;

// load libraries
R__LOAD_LIBRARY(libsenergycorrelator.so)
R__LOAD_LIBRARY(libscorrelatorutilities.so)



// macro body -----------------------------------------------------------------

void RunENCCalculation(const bool doBatch = false, const int verbosity = 0) {

  // input/output files
  const vector<string> inFiles = {
    "../SCorrelatorJetTreeMaker/output/intermediate_merge/pp200py8jet10run15_towerSubAntiKtR04_2024sep28/correlatorJetTree.pp200py8jet10run15_towerSubAntiKtR04_0000to0099.d29m9y2024.root"
  };
  const pair<string, string> outFiles = {
    "twoPoint.pp200py8jet10run15_antiKtTowerR04_packageCalcOnly.refactor_addGlobalCalc_reco.d6m11y2024.root",
    "twoPoint.pp200py8jet10run15_antiKtTowerR04_packageCalcOnly.refactor_addGlobalCalc_true.d6m11y2024.root"
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
