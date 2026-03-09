/// ---------------------------------------------------------------------------
/*! \file   Fun4All_RunCorrelatorJetTreeMakerOnCondor.C
 *  \author Derek Anderson
 *  \date   01.06.2023
 *
 * A F4A macro to run the correlator jet tree maker module over simulation
 * via condor. Initially derived from code by Cameron Dean and Antonio
 * Silva.
 */
/// ---------------------------------------------------------------------------

/****************************/
/*     MDC2 Reco for MDC2   */
/* Cameron Dean, LANL, 2021 */
/*      cdean@bnl.gov       */
/****************************/

#ifndef FUN4ALL_RUNCORRELATORJETTREEMAKERONCONDOR_C
#define FUN4ALL_RUNCORRELATORJETTREEMAKERONCONDOR_C

// standard c includes
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
// module definition
#include <scorrelatorjettreemaker/SCorrelatorJetTreeMaker.h>
// macro options
#include "TopoClusterOptions.h"
#include "JetTreeMakerOptions.h"

// make common namespaces implicit
using namespace std;
using namespace SColdQcdCorrelatorAnalysis;

// load libraries
R__LOAD_LIBRARY(libg4eval.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libparticleflow.so)
R__LOAD_LIBRARY(/sphenix/user/danderson/install/lib/libscorrelatorjettree.so)

// default values
static const string SInListDefault   = "test.list";
static const string SOutDirDefault   = "/sphenix/user/danderson/eec/SCorrelatorJetTreeMaker/submit/ppJet10GeV/output";
static const string SRecoNameDefault = "CorrelatorJetTree";
static const int    NEvtDefault      = 10;
static const int    VerbDefault      = 0;



// macro body -----------------------------------------------------------------

void Fun4All_RunCorrelatorJetTreeOnCondor(
  vector<string> sInputLists = {SInListDefault},
  const int nEvents = NEvtDefault,
  const int verbosity = VerbDefault
) {

  // options ------------------------------------------------------------------

  // tracking/pf options
  const bool   runTracking(false);
  const bool   runParticleFlow(false);
  const bool   doTruthTableReco(false);
  const double nSigma(1.5);

  // get topocluster configurations
  TopoClusterOptions::TopoClusterConfig cfg_topoClustECal = TopoClusterOptions::GetConfig(
    TopoClusterOptions::Combo::ECalOnly,
    verbosity,
    "EcalRawClusterBuilderTopo",
    "TOPOCLUSTER_EMCAL"
  );
  TopoClusterOptions::TopoClusterConfig cfg_topoClustBoth = TopoClusterOptions::GetConfig(
    TopoClusterOptions::Combo::BothCalos,
     verbosity,
    "HcalRawClusterBuilderTopo",
    "TOPOCLUSTER_HCAL"
  );

  // get jet tree maker configuration
  SCorrelatorJetTreeMakerConfig cfg_jetTree = JetTreeMakerOptions::GetConfig(verbosity, outFile);

  // intitialize f4a ----------------------------------------------------------

  // load libraries and create f4a server
  gSystem -> Load("libg4dst.so");
  gSystem -> Load("libFROG.so");

  FROG          *frog      = new FROG();
  Fun4AllServer *ffaServer = Fun4AllServer::instance();

  // figure out folder revisions, file numbers, etc
  string outDir   = SOutDirDefault;
  string recoName = SRecoNameDefault;
  if (outDir.substr(outDir.size() - 1, 1) != "/") {
    outDir += "/";
  }
  outDir += recoName + "/";

  string fileNumber   = sInputLists[0];
  size_t findLastDash = fileNumber.find_last_of("-");
  if (findLastDash != string::npos) {
    fileNumber.erase(0, findLastDash + 1);
  }
  string remove_this = ".list";

  size_t pos = fileNumber.find(remove_this);
  if (pos != string::npos) {
    fileNumber.erase(pos, remove_this.length());
  }

  // create output name
  string outputFileName = "outputData_" + recoName + "_" + fileNumber + ".root";
  string outputRecoDir  = outDir + "/inReconstruction/";
  string makeDirectory  = "mkdir -p " + outputRecoDir;

  system(makeDirectory.c_str());
  string outputRecoFile = outputRecoDir + outputFileName;

  // add all inputs to input managers
  for (unsigned int iMan = 0; iMan < sInputLists.size(); ++iMan) {
    Fun4AllInputManager *inFileMan = new Fun4AllDstInputManager("DSTin_" + to_string(iMan));
    inFileMan -> AddListFile(sInputLists[iMan]);
    ffaServer -> registerInputManager(inFileMan);
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

  // do particle flow reconstruction ------------------------------------------

  // if using particle flow, run pf reconstruction
  if (runParticleFlow) {

    // register topocluster builders
    RawClusterBuilderTopo* ecalClusterBuilder = TopoClusterOptions::CreateBuilder(cfg_topoClustECal);
    RawClusterBuilderTopo* bothClusterBuilder = TopoClusterOptions::CreateBuilder(cfg_topoClustBoth);
    ffaServer -> registerSubsystem(ecalClusterBuilder);
    ffaServer -> registerSubsystem(bothClusterBuilder);

    // do particle flow
    ParticleFlowReco* parFlowReco = new ParticleFlowReco();
    parFlowReco -> set_energy_match_Nsigma(nSigma);
    parFlowReco -> Verbosity(verbosity);
    ffaServer   -> registerSubsystem(parFlowReco);
  }

  // register jet tree maker --------------------------------------------------

  // create correlator jet tree
  SCorrelatorJetTreeMaker* jetTreeMaker = new SCorrelatorJetTreeMaker(cfg_jetTree);
  ffaServer  -> registerSubsystem(jetTreeMaker);

  // run and close f4a --------------------------------------------------------

  // move output and clean up logs
  ifstream file(outputRecoFile.c_str());
  if (file.good()) {
    string moveOutput = "mv " + outputRecoFile + " " + outDir;
    system(moveOutput.c_str());
  } else {
    string rmOutput = "rm " + outDir + "CorrelatorJetTree_" + fileNumber + ".root";
    string rmLog    = "rm /sphenix/user/danderson/eec/submit/logs/" + fileNumber + ".*";
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
