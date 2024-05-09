// ----------------------------------------------------------------------------
// 'RunStandaloneCorrelatorQAModules.C'
// Derek Anderson
// 02.19.2024
//
// A root macro to run correlator QA modules.
// ----------------------------------------------------------------------------

#ifndef RUNSTANDALONECORRELATORQAMODULES_C
#define RUNSTANDALONECORRELATORQAMODULES_C

// c++ utilities
#include <string>
#include <vector>
#include <iostream>
// analysis utilities
#include "CorrelatorQAMakerOptions.h"
//#include "/sphenix/user/danderson/install/include/scorrelatorqamaker/SCorrelatorQAMaker.h"
//#include "/sphenix/user/danderson/install/include/scorrelatorqamaker/SReadLambdaJetTree.h"
#include <scorrelatorqamaker/SCorrelatorQAMaker.h>
#include <scorrelatorqamaker/SReadLambdaJetTree.h>

// make common namespaces implicit
using namespace std;
using namespace SColdQcdCorrelatorAnalysis;

// load libraries
R__LOAD_LIBRARY(/sphenix/user/danderson/install/lib/libscorrelatorqamaker.so)
R__LOAD_LIBRARY(/sphenix/user/danderson/install/lib/libscorrelatorutilities.so)

// default input/output 
static const vector<string> VecInFilesDefault = {
  "../SLambdaJetHunter/output/condor/final_merge/lambdaJetTree_full_withAssocFix.pp200py8jet10run8.d19m2y2024.root"
};
static const vector<string> VecOutFilesDefault = {
  "lambdaJetTreePlots.checkTrigThresholds_ptJet25ptLam02.r04full.pp200py8jet10run8.d22m3y2024.root"
};
static const vector<string> VecOutDirDefault = {
  "ReadLambdaJetTree"
};

// other default arguments
static const int  VerbDefault  = 0;
static const bool DebugDefault = false;



// macro body -----------------------------------------------------------------

void RunStandaloneCorrelatorQAModules(
  const vector<string>& vecOutFiles = VecOutFilesDefault,
  const vector<string>& vecOutDir = VecOutDirDefault,
  const vector<string>& vecInFiles = VecInFilesDefault,
  const int verbosity = VerbDefault,
  const bool debug = DebugDefault
) {

  // options ------------------------------------------------------------------

  // get module configurations
  SReadLambdaJetTreeConfig cfg_readLambdaJetTree = CorrelatorQAMakerOptions::GetReadLambdaJetTreeConfig(vecInFiles.at(0));

  // run qa makers ------------------------------------------------------------

  // instantiate qa maker and plugins
  SCorrelatorQAMaker* maker = new SCorrelatorQAMaker();
  maker -> InitPlugin(cfg_readLambdaJetTree);
  maker -> SetGlobalVerbosity(verbosity);
  maker -> SetGlobalDebug(debug);
  maker -> ReadLambdaJetTree() -> SetOutFile(vecOutFiles.at(0));
  maker -> ReadLambdaJetTree() -> SetOutDir(vecOutDir.at(0));

  // run plugins
  maker -> ReadLambdaJetTree() -> Init();
  maker -> ReadLambdaJetTree() -> Analyze();
  maker -> ReadLambdaJetTree() -> End();

  // exit macro
  return;

}

#endif

// end ------------------------------------------------------------------------
