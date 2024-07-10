// ----------------------------------------------------------------------------
// 'Fun4All_RunLambdaJetHunter.C'
// Derek Anderson
// 01.25.2024
//
// F4A macro the SLambdaJetHunter module.
// ----------------------------------------------------------------------------

#define FUN4ALL_RUNLAMBDAJETHUNTER_C

// c++ utilities
#include <vector>
#include <string>
// f4a/sphenix libraries
#include <FROG.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <g4main/Fun4AllDstPileupInputManager.h>
// module definition
#include <slambdajethunter/SLambdaJetHunter.h>
// macro options
#include "LambdaJetHunterOptions.h"

// make common namespaces implicit
using namespace std;
using namespace SColdQcdCorrelatorAnalysis;

// load libraries
R__LOAD_LIBRARY(libslambdajethunter.so)
R__LOAD_LIBRARY(libscorrelatorutilities.so)

// global constants
static const int            NEvtDefault = 10;
static const int            VerbDefault = 0;
static const string         SOutDefault = "test.root";
static const vector<string> SInDefault  = {
  "DST_GLOBAL_pythia8_Jet10_3MHz-0000000008-04075.root",
  "DST_TRUTH_pythia8_Jet10_3MHz-0000000008-04075.root"
};



// macro body -----------------------------------------------------------------

void Fun4All_RunLambdaJetHunter(
  const vector<string>& sInput    = SInDefault,
  const string          sOutput   = SOutDefault,
  const int             nEvents   = NEvtDefault,
  const int             verbosity = VerbDefault
) {

  // set module configuration
  SLambdaJetHunterConfig config = LambdaJetHunterOptions::GetConfig(verbosity, sOutput);

  // load libraries and create f4a server
  gSystem -> Load("libg4dst.so");
  gSystem -> Load("libFROG.so");

  FROG*          frog      = new FROG();
  Fun4AllServer* ffaServer = Fun4AllServer::instance();
  ffaServer -> Verbosity(verbosity);

  // add input files 
  for (size_t iInput = 0; iInput < sInput.size(); iInput++) {
    Fun4AllDstInputManager* inManager = new Fun4AllDstInputManager("InputDstManager" + to_string(iInput));
    inManager -> AddFile(sInput.at(iInput));
    ffaServer -> registerInputManager(inManager);
  }

  // instantiate & register lambda jet hunter
  SLambdaJetHunter* hunter = new SLambdaJetHunter(config);
  ffaServer -> registerSubsystem(hunter);

  // run reconstruction & close f4a
  ffaServer -> run(nEvents);
  ffaServer -> End();
  delete ffaServer;

  // announce end & exit
  gSystem -> Exit(0);
  return;

}

// end ------------------------------------------------------------------------
