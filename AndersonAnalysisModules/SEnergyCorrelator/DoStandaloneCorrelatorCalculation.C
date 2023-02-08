// 'DoStandaloneCorrelatorCalculation.C'
// Derek Anderson
// 02.02.2023
//
// Use this to run the SEnergyCorrelator
// class in standalone mode.

// standard c includes
#include <string>
#include <cstdlib>
// user includes
#include "/sphenix/u/danderson/install/include/senergycorrelator/SEnergyCorrelator.h"

// load libraries
R__LOAD_LIBRARY(/sphenix/u/danderson/install/lib/libsenergycorrelator.so)

using namespace std;



void DoStandaloneCorrelatorCalculation() {

  // io parameters
  const string inFile("/sphenix/user/danderson/eec/SCorrelatorJetTree/output/condor/final_merge/correlatorJetTree.pp200py8run50_tracks_withQAHists.d2m2y2023.root");
  const string inReco("RecoJetTree");
  const string inTrue("TruthJetTree");
  const string outFile("test.root");

  // misc parameters
  const int  verbosity = 0;
  const bool doDebug   = true;

  // do correlator calculation on reco jets
  SEnergyCorrelator *recoCorrelator = new SEnergyCorrelator("SRecoEnergyCorrelator", false, doDebug);
  recoCorrelator -> SetVerbosity(verbosity);
  recoCorrelator -> SetInputFile(inFile);
  recoCorrelator -> SetInputTree(inReco);
  recoCorrelator -> SetOutputFile(outFile);
  /* TODO set reco calc parameters here */
  recoCorrelator -> Init();
  recoCorrelator -> Analyze();
  recoCorrelator -> End();

  // do correlator calculation on truth jets
  SEnergyCorrelator *trueCorrelator = new SEnergyCorrelator("STrueEnergyCorrelator", false, doDebug);
  trueCorrelator -> SetVerbosity(verbosity);
  trueCorrelator -> SetInputFile(inFile);
  trueCorrelator -> SetInputTree(inTrue);
  trueCorrelator -> SetOutputFile(outFile);
  /* TODO set truth calc parameters here */
  trueCorrelator -> Init();
  trueCorrelator -> Analyze();
  trueCorrelator -> End();

}

// end ------------------------------------------------------------------------
