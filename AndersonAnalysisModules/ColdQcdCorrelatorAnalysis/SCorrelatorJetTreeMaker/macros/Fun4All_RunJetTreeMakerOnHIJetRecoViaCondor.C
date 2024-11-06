/// ---------------------------------------------------------------------------
/*! \file   Fun4All_RunJetTreeMakerOnHIJetRecoViaCondor.C
 *  \author Derek Anderson
 *  \date   09.23.2024
 *
 *  A F4A macro to run the correlator jet tree maker module over the
 *  output of HIJetReco.C over condor. Initially derived from code by
 *  Cameron Dean and Antonio Silva.
 */
/// ---------------------------------------------------------------------------

#ifndef FUN4ALL_RUNJETTREEMAKERONHIJETRECOVIACONDOR_C
#define FUN4ALL_RUNJETTREEMAKERONHIJETRECOVIACONDOR_C

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

// default input/output
static const string InListDefault = "test.list";
static const string OutRecDefault = "CorrelatorJetTree";
static const string OutDirDefault = "/sphenix/user/danderson/eec/SCorrelatorJetTreeMaker/submit/Run15_PPJet10GeV/output";

// other default arguments
static const int    NEvtDefault = 10;
static const int    VerbDefault = 10;
static const size_t NTopoClusts = 2;
static const size_t NTopoPar    = 3;



// macro body -----------------------------------------------------------------

void Fun4All_RunJetTreeMakerOnHIJetRecoViaCondor(
  const int nEvents = NEvtDefault,
  const int verbosity = VerbDefault,
  const vector<string>& inLists = {InListDefault},
  const string outDir = OutDirDefault,
  const string outRec = OutRecDefault
) {

  // point ROOT to my install directory, add analysis libraries
  gInterpreter -> AddIncludePath("/sphenix/u/danderson/install/include");
  R__LOAD_LIBRARY(libscorrelatorutilities.so)
  R__LOAD_LIBRARY(libscorrelatorjettreemaker.so)

  // options ------------------------------------------------------------------

  // tracking/pf/tower options
  const bool   runTracking(false);
  const bool   runParticleFlow(false);
  const bool   doTruthTableReco(false);
  const bool   doEMCalRetower(true);
  const double nSigma(1.5);

  // adjust verbosity
  Enable::VERBOSITY = verbosity;

  // figure out folder revisions, file numbers, etc
  //   - TODO this should get folded into the utilities namespace
  string outDirUse = outDir;
  string recoName  = outRec;
  if (outDirUse.substr(outDirUse.size() - 1, 1) != "/") {
    outDirUse += "/";
  }
  outDirUse += recoName + "/";

  string fileNumber   = inLists[0];
  size_t findLastDash = fileNumber.find_last_of("-");
  if (findLastDash != string::npos) {
    fileNumber.erase(0, findLastDash + 1);
  }
  string remove_this = ".list";

  size_t pos = fileNumber.find(remove_this);
  if (pos != string::npos) {
    fileNumber.erase(pos, remove_this.length());
  }

  // create output names
  string outputFileName = "outputData_" + recoName + "_" + fileNumber + ".root";
  string outputRecoDir  = outDirUse + "/inReconstruction/";
  string makeDirectory  = "mkdir -p " + outputRecoDir;

  system(makeDirectory.c_str());
  string outputRecoFile = outputRecoDir + outputFileName;

  // get jet tree maker configuration
  SCorrelatorJetTreeMakerConfig cfg_jetTree = JetTreeMakerOptions::GetConfig(
    verbosity,
    0.4,
    "AntiKt_Tower_r04_Sub1",
    "AntiKt_Truth_r04",
    outputRecoFile
  );

  // initialize f4a -----------------------------------------------------------

  // load libraries and create f4a server
  gSystem -> Load("libg4dst.so");
  gSystem -> Load("libFROG.so");

  FROG*          frog      = new FROG();
  Fun4AllServer* ffaServer = Fun4AllServer::instance();
  ffaServer -> Verbosity(verbosity);

  // add input files 
  for (size_t iInput = 0; iInput < inLists.size(); iInput++) {
    Fun4AllDstInputManager* inManager = new Fun4AllDstInputManager("InputDstManager" + to_string(iInput));
    inManager -> AddListFile(inLists.at(iInput));
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
  ffaServer -> registerSubsystem(jetTreeMaker);

  // run and close f4a --------------------------------------------------------

  // move output and clean up logs
  //   - TODO this also should be folded into utilities namespace
  ifstream file(outputRecoFile.c_str());
  if (file.good()) {
    string moveOutput = "mv " + outputRecoFile + " " + outDirUse;
    system(moveOutput.c_str());
  } else {
    string rmOutput = "rm " + outDirUse + recoName +  "_" + fileNumber + ".root";
    string rmLog    = "rm " + outDirUse + "/logs/" + fileNumber + ".*";
    system(rmOutput.c_str());
    system(rmLog.c_str());
  }

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
