// ----------------------------------------------------------------------------
// 'Fun4All_RunCorrelatorJetTreeOnCondor.C'
// Derek Anderson
// 01.06.2023
//
// Use this to run the SCorrelatorJetTree
// class via Condor.
//
// Derived from code by Cameron Dean and
// Antonio Silva (thanks!!)
//
// NOTE: jetType sets whether or not jets
// are full (charge + neutral) or charged
//   jetType = 0: charged jets
//   jetType = 1: full jets
// ----------------------------------------------------------------------------

/****************************/
/*     MDC2 Reco for MDC2   */
/* Cameron Dean, LANL, 2021 */
/*      cdean@bnl.gov       */
/****************************/

// standard c includes
#include <string>
#include <cstdlib>
#include <utility>
// f4a/sphenix includes
#include <QA.C>
#include <FROG.h>
#include <fun4all/Fun4AllDstInputManager.h>
// g4 includes
#include <G4_Magnet.C>
#include <G4_Tracking.C>
#include <g4eval/SvtxEvaluator.h>
#include <g4eval/SvtxTruthRecoTableEval.h>
#include <g4main/Fun4AllDstPileupInputManager.h>
// misc includes
#include <caloreco/RawClusterBuilderTopo.h>
#include <particleflowreco/ParticleFlowReco.h>
// user includes
#include </sphenix/user/danderson/install/include/scorrelatorjettree/SCorrelatorJetTree.h>

// load libraries
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libparticleflow.so)
R__LOAD_LIBRARY(/sphenix/user/danderson/install/lib/libscorrelatorjettree.so)

using namespace std;

// global constants
static const string       SInListDefault   = "test.list";
static const string       SOutDirDefault   = "/sphenix/user/danderson/eec/output/condor/";
static const string       SRecoNameDefault = "CorrelatorJetTree";
static const int          NEvtDefault      = 10;
static const int          VerbDefault      = 0;
static const unsigned int NTopoClusts      = 2;
static const unsigned int NTopoPar         = 3;
static const unsigned int NAccept          = 2;



void Fun4All_RunCorrelatorJetTreeOnCondor(vector<string> sInputLists = {SInListDefault}, const int nEvents = NEvtDefault, const int verbosity = VerbDefault) {

  // track & particle flow parameters
  const bool   runTracking(false);
  const double nSigma(1.5);

  // topo cluster parameters
  const double showerR(0.025);
  const double noiseLevels[NTopoPar]   = {0.0025, 0.006, 0.03};
  const double significance[NTopoPar]  = {4.0,    2.0,   0.0};
  const double localMinE[NTopoPar]     = {1.0,    2.0,   0.5};
  const bool   enableHCal[NTopoClusts] = {false, true};
  const bool   enableECal[NTopoClusts] = {true, false};
  const bool   doSplit(true);
  const bool   allowCorners(true);

  // jet tree general parameters
  const bool isMC(true);
  const bool doDebug(true);
  const bool doMatching(true);
  const bool saveDst(true);
  const bool doQuality(true);
  const bool addTracks(true);
  const bool addECal(false);
  const bool addHCal(false);
  const bool addParticleFlow(false);

  // jet tree jet parameters
  const double       jetRes  = 0.4;
  const unsigned int jetType = 0;
  const auto         jetAlgo = SCorrelatorJetTree::ALGO::ANTIKT;
  const auto         jetReco = SCorrelatorJetTree::RECOMB::PT_SCHEME;

  // constituent acceptance
  const pair<double, double> ptParRange    = {0.,   9999.};
  const pair<double, double> etaParRange   = {-1.1, 1.1};
  const pair<double, double> ptTrackRange  = {0.2,  9999.};
  const pair<double, double> etaTrackRange = {-1.1, 1.1};
  const pair<double, double> ptFlowRange   = {0.2,  9999.};
  const pair<double, double> etaFlowRange  = {-1.1, 1.1};
  const pair<double, double> ptECalRange   = {0.3,  9999.};
  const pair<double, double> etaECalRange  = {-1.1, 1.1};
  const pair<double, double> ptHCalRange   = {0.3,  9999.};
  const pair<double, double> etaHCalRange  = {-1.1, 1.1};

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
  SvtxTruthRecoTableEval *tables = new SvtxTruthRecoTableEval();
  tables -> Verbosity(verbosity);
  if (runTracking) {
    ffaServer -> registerSubsystem(tables);
  }

  // build topo clusters
  RawClusterBuilderTopo* ecalClusterBuilder = new RawClusterBuilderTopo("EcalRawClusterBuilderTopo");
  ecalClusterBuilder -> Verbosity(verbosity);
  ecalClusterBuilder -> set_nodename("TOPOCLUSTER_EMCAL");
  ecalClusterBuilder -> set_enable_HCal(enableHCal[0]);
  ecalClusterBuilder -> set_enable_EMCal(enableECal[0]);
  ecalClusterBuilder -> set_noise(noiseLevels[0], noiseLevels[1], noiseLevels[2]);
  ecalClusterBuilder -> set_significance(significance[0], significance[1], significance[2]);
  ecalClusterBuilder -> allow_corner_neighbor(allowCorners);
  ecalClusterBuilder -> set_do_split(doSplit);
  ecalClusterBuilder -> set_minE_local_max(localMinE[0], localMinE[1], localMinE[2]);
  ecalClusterBuilder -> set_R_shower(showerR);
  ffaServer          -> registerSubsystem(ecalClusterBuilder);

  RawClusterBuilderTopo* hcalClusterBuilder = new RawClusterBuilderTopo("HcalRawClusterBuilderTopo");
  hcalClusterBuilder -> Verbosity(verbosity);
  hcalClusterBuilder -> set_nodename("TOPOCLUSTER_HCAL");
  hcalClusterBuilder -> set_enable_HCal(enableHCal[1]);
  hcalClusterBuilder -> set_enable_EMCal(enableECal[1]);
  hcalClusterBuilder -> set_noise(noiseLevels[0], noiseLevels[1], noiseLevels[2]);
  hcalClusterBuilder -> set_significance(significance[0], significance[1], significance[1]);
  hcalClusterBuilder -> allow_corner_neighbor(allowCorners);
  hcalClusterBuilder -> set_do_split(doSplit);
  hcalClusterBuilder -> set_minE_local_max(localMinE[0], localMinE[1], localMinE[2]);
  hcalClusterBuilder -> set_R_shower(showerR);
  ffaServer          -> registerSubsystem(hcalClusterBuilder);

  // do particle flow
  ParticleFlowReco *parFlowReco = new ParticleFlowReco();
  parFlowReco -> set_energy_match_Nsigma(nSigma);
  parFlowReco -> Verbosity(verbosity);
  ffaServer   -> registerSubsystem(parFlowReco);

  // create correlator jet tree
  SCorrelatorJetTree *correlatorJetTree = new SCorrelatorJetTree("SCorrelatorJetTree", outputRecoFile, isMC, doDebug);
  correlatorJetTree -> Verbosity(verbosity);
  correlatorJetTree -> SetDoQualityPlots(doQuality);
  correlatorJetTree -> SetAddTracks(addTracks);
  correlatorJetTree -> SetAddFlow(addParticleFlow);
  correlatorJetTree -> SetAddECal(addECal);
  correlatorJetTree -> SetAddHCal(addHCal);
  correlatorJetTree -> SetParPtRange(ptParRange);
  correlatorJetTree -> SetParEtaRange(etaParRange);
  correlatorJetTree -> SetTrackPtRange(ptTrackRange);
  correlatorJetTree -> SetTrackEtaRange(etaTrackRange);
  correlatorJetTree -> SetFlowPtRange(ptFlowRange);
  correlatorJetTree -> SetFlowEtaRange(etaFlowRange);
  correlatorJetTree -> SetECalPtRange(ptECalRange);
  correlatorJetTree -> SetECalEtaRange(etaECalRange);
  correlatorJetTree -> SetHCalPtRange(ptHCalRange);
  correlatorJetTree -> SetHCalEtaRange(etaHCalRange);
  correlatorJetTree -> SetJetParameters(jetRes, jetType, jetAlgo, jetReco);
  correlatorJetTree -> SetSaveDST(saveDst);
  ffaServer         -> registerSubsystem(correlatorJetTree);

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

// end ------------------------------------------------------------------------
