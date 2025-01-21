/// ---------------------------------------------------------------------------
/*! \file   Fun4All_RunJetTreeMakerOnHIJetReco.C
 *  \author Derek Anderson
 *  \date   08.26.2024
 *
 *  A F4A macro to run the correlator jet tree maker module over the
 *  output of HIJetReco.C. Initially derived from code by Cameron Dean
 *  and Antonio Silva.
 */
/// ---------------------------------------------------------------------------

#ifndef FUN4ALL_RUNJETTREEMAKERONHIJETRECO_C
#define FUN4ALL_RUNJETTREEMAKERONHIJETRECO_C

// c++ utilities
#include <vector>
#include <string>
#include <cstdlib>
#include <utility>
// f4a/sphenix utilities
#include <FROG.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <g4main/Fun4AllDstPileupInputManager.h>
// tracking utilities
#include <g4eval/SvtxEvaluator.h>
#include <g4eval/SvtxTruthRecoTableEval.h>
// calo/pf/jet libraries
#include <jetbackground/RetowerCEMC.h>
#include <caloreco/RawClusterBuilderTopo.h>
#include <particleflowreco/ParticleFlowReco.h>
// module definition
#include <scorrelatorjettreemaker/SCorrelatorJetTreeMaker.h>
// macro options
#include "JetTreeMakerOptions.h"

// sphenix macros
#include <G4_Magnet.C>
#include <G4_TrkrSimulation.C>
#include <HIJetReco.C>
#include <Trkr_QA.C>
#include <Trkr_Reco.C>
#include <Trkr_Eval.C>
#include <Trkr_RecoInit.C>
#include <Trkr_Clustering.C>
#include <Trkr_Diagnostics.C>

// make common namespaces implicit
using namespace std;
using namespace SColdQcdCorrelatorAnalysis;

// load libraries
R__LOAD_LIBRARY(libg4eval.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libparticleflow.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libscorrelatorutilities.so)
R__LOAD_LIBRARY(libscorrelatorjettreemaker.so)

// default input/output
static const vector<string> VecInFilesDefault = {
  "DST_GLOBAL_pythia8_Jet10_2MHz-0000000015-000006.root",
  "DST_TRKR_G4HIT_pythia8_Jet10_2MHz-0000000015-000006.root",
  "DST_TRACKSEEDS_pythia8_Jet10_2MHz-0000000015-000006.root",
  "DST_TRKR_CLUSTER_pythia8_Jet10_2MHz-0000000015-000006.root",
  "DST_TRACKS_pythia8_Jet10_2MHz-0000000015-000006.root",
  "DST_CALO_G4HIT_pythia8_Jet10_2MHz-0000000015-000006.root",
  "DST_CALO_CLUSTER_pythia8_Jet10_2MHz-0000000015-000006.root",
  "DST_TRUTH_G4HIT_pythia8_Jet10_2MHz-0000000015-000006.root",
  "DST_TRUTH_pythia8_Jet10_2MHz-0000000015-000006.root"
};
static const string OutFileDefault = "test.root";

// other default arguments
static const int    NEvtDefault = 10;
static const int    VerbDefault = 10;
static const size_t NTopoClusts = 2;
static const size_t NTopoPar    = 3;



// macro body -----------------------------------------------------------------

void Fun4All_RunJetTreeMakerOnHIJetReco(
  const vector<string>& vecInFiles = VecInFilesDefault,
  const string outFile = OutFileDefault,
  const int nEvents = NEvtDefault,
  const int verbosity = VerbDefault
) {

  // options ------------------------------------------------------------------

  // tracking/pf/tower options
  const bool   runTracking(false);
  const bool   runParticleFlow(false);
  const bool   doTruthTableReco(false);
  const bool   doEMCalRetower(true);
  const double nSigma(1.5);

  // adjust verbosity
  Enable::VERBOSITY = verbosity;

  // get jet tree maker configuration
  SCorrelatorJetTreeMakerConfig cfg_jetTree = JetTreeMakerOptions::GetConfig(
    verbosity,
    0.4,
    "AntiKt_Tower_r04_Sub1",
    "AntiKt_Truth_r04",
    outFile
  );

  // initialize f4a -----------------------------------------------------------

  // load libraries and create f4a server
  gSystem -> Load("libg4dst.so");
  gSystem -> Load("libFROG.so");

  FROG*          frog      = new FROG();
  Fun4AllServer* ffaServer = Fun4AllServer::instance();
  ffaServer -> Verbosity(verbosity);

  // add input files 
  for (size_t iInput = 0; iInput < vecInFiles.size(); iInput++) {
    Fun4AllDstInputManager* inManager = new Fun4AllDstInputManager("InputDstManager" + to_string(iInput));
    inManager -> AddFile(vecInFiles.at(iInput));
    ffaServer -> registerInputManager(inManager);
  }

  // do jet reconstruction ----------------------------------------------------

  // set options and run jet reco
  HIJETS::is_pp = true;
  if (cfg_jetTree.isSimulation) {
    Enable::HIJETS_MC    = true;
    Enable::HIJETS_TRUTH = true;
  } else {
    Enable::HIJETS_MC    = false;
    Enable::HIJETS_TRUTH = false;
  }
  HIJetReco();

  // register jet tree maker --------------------------------------------------

  // create correlator jet tree
  SCorrelatorJetTreeMaker* jetTreeMaker = new SCorrelatorJetTreeMaker(cfg_jetTree);
  ffaServer  -> registerSubsystem(jetTreeMaker);

  // run and close f4a --------------------------------------------------------

  // run reconstruction & close f4a
  ffaServer -> run(nEvents);
  ffaServer -> End();
  delete ffaServer;

  // announce end & exit
  gSystem -> Exit(0);
  return;

}

#endif

// end ------------------------------------------------------------------------
