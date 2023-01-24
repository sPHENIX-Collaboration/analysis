// 'Fun4All_RunCorrelatorJetTree.C'
// Derek Anderson
// 12.11.2022
//
// Use this to run the SCorrelatorJetTree
// class.
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
static const string       SInHitsDefault = "/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/g4hits/G4Hits_pythia8_pp_mb-0000000050-03954.root";
static const string       SInCaloDefault = "/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/calocluster/DST_CALO_CLUSTER_pythia8_pp_mb_3MHz-0000000050-03954.root";
static const string       SInSeedDefault = "/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trackseeds/DST_TRACKSEEDS_pythia8_pp_mb_3MHz-0000000050-03954.root";
static const string       SInTrksDefault = "/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/tracks/DST_TRACKS_pythia8_pp_mb_3MHz-0000000050-03954.root";
static const string       SInTrueDefault = "/sphenix/lustre01/sphnxpro/mdc2/pythia8_pp_mb/trkrhit/DST_TRUTH_pythia8_pp_mb_3MHz-0000000050-03954.root";
static const string       SOutDefault    = "oops.root";
static const int          NEvtDefault    = 10;
static const int          VerbDefault    = 0;
static const unsigned int NTopoClusts    = 2;
static const unsigned int NTopoPar       = 3;
static const unsigned int NAccept        = 2;



void Fun4All_RunCorrelatorJetTree(const string sInHits = SInHitsDefault, const string sInCalo = SInCaloDefault, const string sInSeed = SInSeedDefault, const string sInTrks = SInTrksDefault, const string sInTrue = SInTrueDefault, const string sOutput = SOutDefault, const int nEvents = NEvtDefault, const int verbosity = VerbDefault) {

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
  const bool   doQuality(true);
  const bool   addTracks(true);
  const bool   addEMClusters(true);
  const bool   addHClusters(true);
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
  se -> Verbosity(verbosity);

  // add input files
  Fun4AllInputManager *inHitsMan = new Fun4AllDstInputManager("InputDstManager_G4Hits");
  Fun4AllInputManager *inCaloMan = new Fun4AllDstInputManager("InputDstManager_CaloClusts");
  Fun4AllInputManager *inSeedMan = new Fun4AllDstInputManager("InputDstManager_TrackSeeds");
  Fun4AllInputManager *inTrksMan = new Fun4AllDstInputManager("InputDstManager_Tracks");
  Fun4AllInputManager *inTrueMan = new Fun4AllDstInputManager("InputDstManager_Truth");
  inHitsMan -> AddFile(sInHits);
  inCaloMan -> AddFile(sInCalo);
  inSeedMan -> AddFile(sInSeed);
  inTrksMan -> AddFile(sInTrks);
  inTrueMan -> AddFile(sInTrue);
  se        -> registerInputManager(inHitsMan);
  se        -> registerInputManager(inCaloMan);
  if (isMC) {
    se -> registerInputManager(inSeedMan);
    se -> registerInputManager(inTrksMan);
    se -> registerInputManager(inTrueMan);
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
  SCorrelatorJetTree *correlatorJetTree = new SCorrelatorJetTree("SCorrelatorJetTree", sOutput, isMC, doDebug);
  correlatorJetTree -> Verbosity(verbosity);
  correlatorJetTree -> setDoQualityPlots(doQuality);
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

  // run reconstruction & close f4a
  se -> run(nEvents);
  se -> End();
  delete se;

  // announce end & exit
  gSystem -> Exit(0);
  return;

}

// end ------------------------------------------------------------------------
