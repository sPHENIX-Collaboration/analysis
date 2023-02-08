// 'Fun4All_RunCorrelatorJetTreeOnCondor.C'
// Derek Anderson
// 01.06.2023
//
// Use this to run the SCorrelatorJetTree
// class via Condor.
//
// Derived from code by Cameron Dean and
// Antonio Silva (thanks!!)

/****************************/
/*     MDC2 Reco for MDC2   */
/* Cameron Dean, LANL, 2021 */
/*      cdean@bnl.gov       */
/****************************/

// standard c includes
#include <string>
#include <cstdlib>
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
#include </sphenix/u/danderson/install/include/scorrelatorjettree/SCorrelatorJetTree.h>

// load libraries
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libparticleflow.so)
R__LOAD_LIBRARY(/sphenix/u/danderson/install/lib/libscorrelatorjettree.so)

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

  // jet tree parameters
  const bool   isMC(true);
  const bool   doDebug(true);
  const bool   saveDst(true);
  const bool   addTracks(false);
  const bool   addEMClusters(false);
  const bool   addHClusters(false);
  const bool   addParticleFlow(true);
  const double ptTrackAccept[NAccept]     = {0.2,  9999.};
  const double ptEMClustAccept[NAccept]   = {0.3,  9999.};
  const double ptHClustAccept[NAccept]    = {0.3,  9999.};
  const double etaTrackAccept[NAccept]    = {-1.1, 1.1};
  const double etaEMClustAccept[NAccept]  = {-1.1, 1.1};
  const double etaHClustAccept[NAccept]   = {-1.1, 1.1};
  const double etaPartFlowAccept[NAccept] = {-1.1, 1.1};

  // jet tree jet parameters
  const double jetRes  = 0.3;
  const auto   jetAlgo = SCorrelatorJetTree::ALGO::ANTIKT;
  const auto   jetReco = SCorrelatorJetTree::RECOMB::PT_SCHEME;

  // load libraries and create f4a server
  gSystem -> Load("libg4dst.so");
  gSystem -> Load("libFROG.so");

  FROG          *fr = new FROG();
  Fun4AllServer *se = Fun4AllServer::instance();

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
    se        -> registerInputManager(inFileMan);
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
    se -> registerSubsystem(tables);
  }

  // build topo clusters
  RawClusterBuilderTopo* ClusterBuilder1 = new RawClusterBuilderTopo("EcalRawClusterBuilderTopo");
  ClusterBuilder1 -> Verbosity(verbosity);
  ClusterBuilder1 -> set_nodename("TOPOCLUSTER_EMCAL");
  ClusterBuilder1 -> set_enable_HCal(enableHCal[0]);
  ClusterBuilder1 -> set_enable_EMCal(enableECal[0]);
  ClusterBuilder1 -> set_noise(noiseLevels[0], noiseLevels[1], noiseLevels[2]);
  ClusterBuilder1 -> set_significance(significance[0], significance[1], significance[2]);
  ClusterBuilder1 -> allow_corner_neighbor(allowCorners);
  ClusterBuilder1 -> set_do_split(doSplit);
  ClusterBuilder1 -> set_minE_local_max(localMinE[0], localMinE[1], localMinE[2]);
  ClusterBuilder1 -> set_R_shower(showerR);
  se              -> registerSubsystem(ClusterBuilder1);

  RawClusterBuilderTopo* ClusterBuilder2 = new RawClusterBuilderTopo("HcalRawClusterBuilderTopo");
  ClusterBuilder2 -> Verbosity(verbosity);
  ClusterBuilder2 -> set_nodename("TOPOCLUSTER_HCAL");
  ClusterBuilder2 -> set_enable_HCal(enableHCal[1]);
  ClusterBuilder2 -> set_enable_EMCal(enableECal[1]);
  ClusterBuilder2 -> set_noise(noiseLevels[0], noiseLevels[1], noiseLevels[2]);
  ClusterBuilder2 -> set_significance(significance[0], significance[1], significance[1]);
  ClusterBuilder2 -> allow_corner_neighbor(allowCorners);
  ClusterBuilder2 -> set_do_split(doSplit);
  ClusterBuilder2 -> set_minE_local_max(localMinE[0], localMinE[1], localMinE[2]);
  ClusterBuilder2 -> set_R_shower(showerR);
  se              -> registerSubsystem(ClusterBuilder2);

  // do particle flow
  ParticleFlowReco *pfr = new ParticleFlowReco();
  pfr -> set_energy_match_Nsigma(nSigma);
  pfr -> Verbosity(verbosity);
  se  -> registerSubsystem(pfr);

  // create correlator jet tree
  SCorrelatorJetTree *correlatorJetTree = new SCorrelatorJetTree("SCorrelatorJetTree", outputRecoFile, isMC, doDebug);
  correlatorJetTree -> Verbosity(verbosity);
  correlatorJetTree -> setAddTracks(addTracks);
  correlatorJetTree -> setAddEMCalClusters(addEMClusters);
  correlatorJetTree -> setAddHCalClusters(addHClusters);
  correlatorJetTree -> setAddParticleFlow(addParticleFlow);
  correlatorJetTree -> setTrackPtAcc(ptTrackAccept[0], ptTrackAccept[1]);
  correlatorJetTree -> setEMCalClusterPtAcc(ptEMClustAccept[0], ptEMClustAccept[1]);
  correlatorJetTree -> setHCalClusterPtAcc(ptHClustAccept[0], ptHClustAccept[1]);
  correlatorJetTree -> setTrackEtaAcc(etaTrackAccept[0], etaTrackAccept[1]);
  correlatorJetTree -> setEMCalClusterEtaAcc(etaEMClustAccept[0], etaEMClustAccept[1]);
  correlatorJetTree -> setHCalClusterEtaAcc(etaHClustAccept[0], etaHClustAccept[1]);
  correlatorJetTree -> setParticleFlowEtaAcc(etaPartFlowAccept[0], etaPartFlowAccept[1]);
  correlatorJetTree -> setJetParameters(jetRes, jetAlgo, jetReco);
  correlatorJetTree -> setSaveDST(saveDst);
  se                -> registerSubsystem(correlatorJetTree);

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
  se -> run(nEvents);
  se -> End();
  delete se;

  // announce end & exit
  gSystem -> Exit(0);
  return;

}

// end ------------------------------------------------------------------------
