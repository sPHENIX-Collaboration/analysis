// ----------------------------------------------------------------------------
// 'Fun4All_RunCorrelatorQAModules.C'
// Derek Anderson
// 12.27.2023
//
// A F4A macro to run correlator QA modules.
// ----------------------------------------------------------------------------

#ifndef FUN4ALL_RUNCORRELATORQAMODULES_C
#define FUN4ALL_RUNCORRELATORQAMODULES_C

// c++ utilities
#include <vector>
#include <string>
#include <cstdlib>
#include <utility>
// f4a/sphenix utilities
#include <FROG.h>
#include <G4_Magnet.C>
#include <fun4all/Fun4AllDstInputManager.h>
#include <g4main/Fun4AllDstPileupInputManager.h>
// tracking utilities
#include <Trkr_QA.C>
#include <Trkr_Reco.C>
#include <Trkr_Eval.C>
#include <Trkr_RecoInit.C>
#include <Trkr_Clustering.C>
#include <Trkr_Diagnostics.C>
#include <G4_TrkrSimulation.C>
#include <g4eval/SvtxEvaluator.h>
#include <g4eval/SvtxTruthRecoTableEval.h>
// calo/pf libraries
#include <caloreco/RawClusterBuilderTopo.h>
#include <particleflowreco/ParticleFlowReco.h>
// plugin definitions
#include <scorrelatorqamaker/SMakeClustQATree.h>
#include <scorrelatorqamaker/SCheckTrackPairs.h>
#include <scorrelatorqamaker/SMakeTrackQATuple.h>
#include <scorrelatorqamaker/SCorrelatorQAMaker.h>
// macro options
#include "CorrelatorQAMakerOptions.h"

// make common namespaces implicit
using namespace std;
using namespace SColdQcdCorrelatorAnalysis;

// load libraries
R__LOAD_LIBRARY(libg4eval.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libparticleflow.so)
R__LOAD_LIBRARY(libscorrelatorqamaker.so)
R__LOAD_LIBRARY(libscorrelatorutilities.so)

// default input/output 
static const vector<string> VecInFilesDefault = {
  "DST_GLOBAL_pythia8_Jet10_sHijing_pAu_0_10fm_500kHz_bkg_0_10fm-0000000009-00009.root",
  "DST_TRKR_G4HIT_pythia8_Jet10_sHijing_pAu_0_10fm_500kHz_bkg_0_10fm-0000000009-00009.root",
  "DST_TRACKSEEDS_pythia8_Jet10_sHijing_pAu_0_10fm_500kHz_bkg_0_10fm-0000000009-00009.root",
  "DST_TRKR_CLUSTER_pythia8_Jet10_sHijing_pAu_0_10fm_500kHz_bkg_0_10fm-0000000009-00009.root",
  "DST_TRACKS_pythia8_Jet10_sHijing_pAu_0_10fm_500kHz_bkg_0_10fm-0000000009-00009.root",
  "DST_CALO_G4HIT_pythia8_Jet10_sHijing_pAu_0_10fm_500kHz_bkg_0_10fm-0000000009-00009.root",
  "DST_CALO_CLUSTER_pythia8_Jet10_sHijing_pAu_0_10fm_500kHz_bkg_0_10fm-0000000009-00009.root",
  "DST_TRUTH_G4HIT_pythia8_Jet10_sHijing_pAu_0_10fm_500kHz_bkg_0_10fm-0000000009-00009.root",
  "DST_TRUTH_pythia8_Jet10_sHijing_pAu_0_10fm_500kHz_bkg_0_10fm-0000000009-00009.root"
};
static const vector<string> VecOutFilesDefault = {
  "test.root",
  "test1.root",
  "test2.root"
};
static const vector<string> VecOutDirDefault = {
  "CheckTrackPairs",
  "TrackQATuple",
  "ClustQATree"
};

// other default arguments
static const int  NEvtDefault  = 10;
static const int  VerbDefault  = 0;
static const bool DebugDefault = false;



// macro body -----------------------------------------------------------------

void Fun4All_RunCorrelatorQAModules(
  const vector<string>& vecOutFiles = VecOutFilesDefault,
  const vector<string>& vecOutDir = VecOutDirDefault,
  const vector<string>& vecInFiles = VecInFilesDefault,
  const int nEvents = NEvtDefault,
  const int verbosity = VerbDefault,
  const bool debug = DebugDefault
) {

  // options ------------------------------------------------------------------

  // track & particle flow parameters
  const bool runTracking(false);
  const bool doTruthTableReco(false);

  // get module configurations
  SCheckTrackPairsConfig  cfg_checkTrackPairs  = CorrelatorQAMakerOptions::GetCheckTrackPairsConfig();
  SMakeTrackQATupleConfig cfg_makeTrackQATuple = CorrelatorQAMakerOptions::GetMakeTrackQATupleConfig();
  SMakeClustQATreeConfig  cfg_makeClustQATree  = CorrelatorQAMakerOptions::GetMakeClustQATreeConfig();

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

  // set up tracking and truth tables -----------------------------------------

  // run the tracking if not already done
  if (runTracking) {

    // enable mms
    Enable::MICROMEGAS = true;

    // initialize magnetic field
    G4MAGNET::magfield_rescale = 1.;
    MagnetInit();
    MagnetFieldInit();

    // initialize tracker cells
    Mvtx_Cells();
    Intt_Cells();
    TPC_Cells();
    Micromegas_Cells();

    // initialize tracking 
    TrackingInit();

    // do tracker clustering & reconstruction
    Mvtx_Clustering();
    Intt_Clustering();
    TPC_Clustering();
    Micromegas_Clustering();
    Tracking_Reco();
  }

  // construct track/truth table
  if (doTruthTableReco) {
    SvtxTruthRecoTableEval *tables = new SvtxTruthRecoTableEval();
    tables -> Verbosity(verbosity);
    if (runTracking) {
      ffaServer -> registerSubsystem(tables);
    }
  }

  // register qa makers -------------------------------------------------------

  // instantiate qa maker and plugins
  SCorrelatorQAMaker* maker = new SCorrelatorQAMaker();
  maker     -> InitPlugin(cfg_checkTrackPairs,  "CheckTrackPairs");
  maker     -> InitPlugin(cfg_makeTrackQATuple, "MakeTrackQATuple");
  maker     -> InitPlugin(cfg_makeClustQATree,  "MakeClustQATree");
  //maker     -> SetGlobalOutFile(vecOutFiles.at(0));  // FIXME use one file once file opening is fixed
  maker     -> CheckTrackPairs()  -> SetOutFile(vecOutFiles.at(0));
  maker     -> MakeTrackQATuple() -> SetOutFile(vecOutFiles.at(1));
  maker     -> MakeClustQATree()  -> SetOutFile(vecOutFiles.at(2));
  maker     -> SetGlobalVerbosity(verbosity);
  maker     -> SetGlobalDebug(debug);
  maker     -> CheckTrackPairs()  -> SetOutDir(vecOutDir.at(0));
  maker     -> MakeTrackQATuple() -> SetOutDir(vecOutDir.at(1));
  maker     -> MakeClustQATree()  -> SetOutDir(vecOutDir.at(2));
  ffaServer -> registerSubsystem(maker -> CheckTrackPairs());
  ffaServer -> registerSubsystem(maker -> MakeTrackQATuple());
  ffaServer -> registerSubsystem(maker -> MakeClustQATree());

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
