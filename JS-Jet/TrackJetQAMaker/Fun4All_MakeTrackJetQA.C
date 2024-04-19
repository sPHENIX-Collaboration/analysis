// ----------------------------------------------------------------------------
// 'Fun4All_MakeTrackJetQA.C'
// Derek Anderson
// 03.25.2024
//
// A Fun4All macro to run the TrackJetQAMaker module.
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
// jet base software
#include <jetbase/Jet.h>
#include <jetbase/JetReco.h>
#include <jetbase/FastJetAlgo.h>
#include <jetbase/TrackJetInput.h>
// module definition
#include </sphenix/user/danderson/install/include/trackjetqamaker/TrackJetQAMaker.h>
#include </sphenix/user/danderson/install/include/trackjetqamaker/TrackJetQAMakerConfig.h>
// f4a macros
#include <G4_Magnet.C>
#include <G4_ActsGeom.C>
#include <GlobalVariables.C>
#include <Trkr_Clustering.C>

// load libraries
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(/sphenix/user/danderson/install/lib/libtrackjetqamaker.so)



// macro body -----------------------------------------------------------------

void Fun4All_MakeTrackJetQA(
  const int         verb           = 1,
  const int64_t     nEvts          = 1000,
  const std::string outFile        = "test.root",
  const std::string inTrkDSTs      = "input/dst_tracks.list",
  const std::string inTrkHitDSTs   = "input/dst_trkr_hit.list",
  const std::string inTrkClustDSTs = "input/dst_trkr_cluster.list"
) {

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
  Fun4AllRunNodeInputManager* geoManager   = new Fun4AllRunNodeInputManager("GeometryManager");
  trkManager   -> AddListFile(inTrkDSTs.data(), 1);
  hitManager   -> AddListFile(inTrkHitDSTs.data(), 1);
  clustManager -> AddListFile(inTrkClustDSTs.data(), 1);
  geoManager   -> AddFile(inGeoFile.data());
  f4a -> registerInputManager(trkManager);
  f4a -> registerInputManager(hitManager);
  f4a -> registerInputManager(clustManager);
  f4a -> registerInputManager(geoManager);

  // initialize acts geometry
  G4TPC::tpc_drift_velocity_reco = (8.0 / 1000) * 107.0 / 105.0;
  G4MAGNET::magfield = "0.01";
  G4MAGNET::magfield_rescale = 1;
  ACTSGEOM::ActsGeomInit();

  // run jet reconstruction
  JetReco* reco = new JetReco();
  reco -> add_input(new TrackJetInput(Jet::SRC::TRACK));
  reco -> add_algo(new FastJetAlgo(Jet::ALGO::ANTIKT, 0.4), "AntiKt_Track_r04");
  reco -> set_algo_node("ANTIKT");
  reco -> set_input_node("TRACK");
  reco -> Verbosity(verb);
  f4a  -> registerSubsystem(reco);

  // initialize and register qa module
  TrackJetQAMaker* maker = new TrackJetQAMaker("TrackJetQAMaker", outFile);
  maker -> Configure(
    {
      .verbose   = verb,
      .doDebug   = true,
      .doHitQA   = true,
      .doClustQA = true,
      .doTrackQA = true,
      .doJetQA   = true
    }
  );
  f4a -> registerSubsystem(maker);

  // run4all
  f4a -> run(nEvts);
  f4a -> End();
  delete f4a;

  // close and exit
  gSystem -> Exit(0);
  return;

}

// end ------------------------------------------------------------------------
