// ----------------------------------------------------------------------------
// 'Fun4All_MakeTrksInJetQA.C'
// Derek Anderson
// 03.25.2024
//
// A Fun4All macro to run the TrksInJetQA module.
// ----------------------------------------------------------------------------

#define FUN4ALL_MAKETRACKSINJETSQA_C

// c++ utilities
#include <string>
#include <vector>
// fun4all libraries
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
// f4a modules
#include <ffamodules/CDBInterface.h>
// phool libraries
#include <phool/recoConsts.h>
#include <phool/PHRandomSeed.h>
// qa utils
#include <qautils/QAHistManagerDef.h>
// jet base software
#include <jetbase/Jet.h>
#include <jetbase/JetReco.h>
#include <jetbase/FastJetAlgo.h>
#include <jetbase/TrackJetInput.h>
// module definition
#include </sphenix/u/danderson/install/include/trksinjetqa/TrksInJetQA.h>
// f4a macros
#include <G4_Magnet.C>
#include <G4_ActsGeom.C>
#include <GlobalVariables.C>
#include <Trkr_Clustering.C>

// load libraries
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libqautils.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(/sphenix/u/danderson/install/lib/libtrksinjetqa.so)



// macro body -----------------------------------------------------------------

void Fun4All_MakeTrksInJetQA(
  const int         verb           = 10,
  const int64_t     nEvts          = 10,
  const std::string outFileBase    = "test",
  const std::string inTrkDSTs      = "input/pp200py8run11jet30.dst_tracks.list",
  const std::string inTrkHitDSTs   = "input/pp200py8run11jet30.dst_trkr_hit.list",
  const std::string inTrkClustDSTs = "input/pp200py8run11jet30.dst_trkr_cluster.list",
  const std::string inCalClustDSTs = "input/pp200py8run11jet30.dst_calo_cluster.list"
) {

  // initialize fun4all -------------------------------------------------------

  // grab instances of fun4all, etc
  Fun4AllServer* f4a = Fun4AllServer::instance();
  CDBInterface*  cdb = CDBInterface::instance();
  recoConsts*    rc  = recoConsts::instance();

  // turn on cdb
  Enable::CDB = true;

  // set cdb tags
  rc -> set_StringFlag("CDB_GLOBALTAG", "ProdA_2023");
  rc -> set_uint64Flag("TIMESTAMP", 6);

  // get url of geo file
  const std::string inGeoFile = cdb -> getUrl("Tracking_Geometry");

  // set up input managers
  Fun4AllDstInputManager*     trkManager   = new Fun4AllDstInputManager("TrackDstManager");
  Fun4AllDstInputManager*     hitManager   = new Fun4AllDstInputManager("TrackHitDstManager");
  Fun4AllDstInputManager*     clustManager = new Fun4AllDstInputManager("TrackClusterDstManager");
  Fun4AllDstInputManager*     caloManager  = new Fun4AllDstInputManager("CaloClusterDstManager");
  Fun4AllRunNodeInputManager* geoManager   = new Fun4AllRunNodeInputManager("GeometryManager");
  trkManager   -> AddListFile(inTrkDSTs.data(), 1);
  hitManager   -> AddListFile(inTrkHitDSTs.data(), 1);
  clustManager -> AddListFile(inTrkClustDSTs.data(), 1);
  caloManager  -> AddListFile(inCalClustDSTs.data(), 1);
  geoManager   -> AddFile(inGeoFile.data());
  f4a -> registerInputManager(trkManager);
  f4a -> registerInputManager(hitManager);
  f4a -> registerInputManager(clustManager);
  f4a -> registerInputManager(caloManager);
  f4a -> registerInputManager(geoManager);

  // initialize acts geometry
  G4TPC::tpc_drift_velocity_reco = (8.0 / 1000) * 107.0 / 105.0;
  G4MAGNET::magfield = "0.01";
  G4MAGNET::magfield_rescale = 1;
  ACTSGEOM::ActsGeomInit();

  // register jet finders -----------------------------------------------------

  // run track jet reconstruction
  JetReco* trkJetFinder = new JetReco();
  trkJetFinder -> add_input(new TrackJetInput(Jet::SRC::TRACK));
  trkJetFinder -> add_algo(new FastJetAlgo(Jet::ALGO::ANTIKT, 0.4), "AntiKt_Track_r04");
  trkJetFinder -> set_algo_node("ANTIKT");
  trkJetFinder -> set_input_node("TRACK");
  trkJetFinder -> Verbosity(verb);
  f4a  -> registerSubsystem(trkJetFinder);

  // run cluster jet reconstruction
  JetReco* clustJetFinder = new JetReco();
  clustJetFinder -> add_input(new TrackJetInput(Jet::SRC::CEMC_CLUSTER));
  clustJetFinder -> add_input(new TrackJetInput(Jet::SRC::HCALIN_CLUSTER));
  clustJetFinder -> add_input(new TrackJetInput(Jet::SRC::HCALOUT_CLUSTER));
  clustJetFinder -> add_algo(new FastJetAlgo(Jet::ALGO::ANTIKT, 0.4), "AntiKt_Cluster_r04");
  clustJetFinder -> set_algo_node("ANTIKT");
  clustJetFinder -> set_input_node("CLUSTER");
  clustJetFinder -> Verbosity(verb);
  f4a  -> registerSubsystem(clustJetFinder);

  // example of how to use modue as a plain SubsysReco module -----------------

  // construct output file names
  const std::string outTrkFile = outFileBase + ".track.root";
  const std::string outCalFile = outFileBase + ".clust.root";

  // initialize and register track jet qa module
  TrksInJetQA* trkJetQAReco = new TrksInJetQA("TrksInJetQA_TrackJets");
  trkJetQAReco -> SetOutFileName(outTrkFile);
  trkJetQAReco -> SetHistSuffix("TrackJetRecoReco");
  trkJetQAReco -> Configure(
    {
      .outMode     = TrksInJetQA::OutMode::File,
      .verbose     = verb,
      .doDebug     = true,
      .doInclusive = true,
      .doInJet     = true,
      .doHitQA     = true,
      .doClustQA   = true,
      .doTrackQA   = true,
      .doJetQA     = true,
      .rJet        = 0.4,
      .jetInNode   = "AntiKt_Track_r04"
    }
  );
  f4a -> registerSubsystem(trkJetQAReco);

  // initialize and register track jet qa module
  TrksInJetQA* clustJetQAReco = new TrksInJetQA("TrksInJetQA_ClustJets");
  clustJetQAReco -> SetOutFileName(outCalFile);
  clustJetQAReco -> SetHistSuffix("ClustJetRecoReco");
  clustJetQAReco -> Configure(
    {
      .outMode     = TrksInJetQA::OutMode::File,
      .verbose     = verb,
      .doDebug     = true,
      .doInclusive = true,
      .doInJet     = true,
      .doHitQA     = true,
      .doClustQA   = true,
      .doTrackQA   = true,
      .doJetQA     = true,
      .rJet        = 0.4,
      .jetInNode   = "AntiKt_Cluster_r04"
    }
  );
  f4a -> registerSubsystem(clustJetQAReco);

  // example of how to use module with qa framework ---------------------------

  const std::string outQAFile = outFileBase + ".qa.root";

  // initialize and register track jet qa module
  TrksInJetQA* trkJetQANode = new TrksInJetQA("TrksInJetQANode_TrackJets");
  trkJetQANode -> SetHistSuffix("TrackJetQANode");
  trkJetQANode -> Configure(
    {
      .outMode     = TrksInJetQA::OutMode::QA,
      .verbose     = verb,
      .doDebug     = true,
      .doInclusive = true,
      .doInJet     = true,
      .doHitQA     = true,
      .doClustQA   = true,
      .doTrackQA   = true,
      .doJetQA     = true,
      .rJet        = 0.4,
      .jetInNode   = "AntiKt_Track_r04"
    }
  );
  f4a -> registerSubsystem(trkJetQANode);

  // initialize and register track jet qa module
  TrksInJetQA* clustJetQANode = new TrksInJetQA("TrksInJetQANode_ClustJets");
  clustJetQANode -> SetHistSuffix("ClustJetQANode");
  clustJetQANode -> Configure(
    {
      .outMode     = TrksInJetQA::OutMode::QA,
      .verbose     = verb,
      .doDebug     = true,
      .doInclusive = true,
      .doInJet     = true,
      .doHitQA     = true,
      .doClustQA   = true,
      .doTrackQA   = true,
      .doJetQA     = true,
      .rJet        = 0.4,
      .jetInNode   = "AntiKt_Cluster_r04"
    }
  );
  f4a -> registerSubsystem(clustJetQANode);

  // run  modules and exit ----------------------------------------------------

  // run4all
  f4a -> run(nEvts);
  f4a -> End();

  // TEST
  std::cout << "TEST nhists in manager = " << QAHistManagerDef::getHistoManager() -> nHistos() << std::endl;

  // save qa output and exit
  QAHistManagerDef::saveQARootFile(outQAFile);
  delete f4a;

  // close and exit
  gSystem -> Exit(0);
  return;

}

// end ------------------------------------------------------------------------
