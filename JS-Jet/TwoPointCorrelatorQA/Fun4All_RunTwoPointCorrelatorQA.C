// ----------------------------------------------------------------------------
// 'Fun4All_RunTwoPointCorrelatorQA.C'
// Derek Anderson
// 05.14.2024
//
// A Fun4All macro to run the TwoPointCorrelatorQA module
// ----------------------------------------------------------------------------

#define FUN4ALL_RUNTWOPOINTCORRELATORQA_C

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
#include </sphenix/user/danderson/install/include/trksinjetqa/TwoPointCorrelatorQA.h>
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
  const std::string inCalClustDSTs = "input/pp200py8run11jet30.dst_calo_cluster.list"
) {`

  // initialize fun4all -------------------------------------------------------

  // grab instances of fun4all, etc
  Fun4AllServer* f4a = Fun4AllServer::instance();

  // set up input managers
  Fun4AllDstInputManager* trkManager   = new Fun4AllDstInputManager("TrackDstManager");
  Fun4AllDstInputManager* caloManager  = new Fun4AllDstInputManager("CaloClusterDstManager");
  trkManager  -> AddListFile(inTrkDSTs.data(), 1);
  caloManager -> AddListFile(inCalClustDSTs.data(), 1);
  f4a -> registerInputManager(trkManager);
  f4a -> registerInputManager(caloManager);

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

  // set up correlator qa module ----------------------------------------------

  TwoPointCorrelatorQA* twoCorrTrkQA = new TwoPointCorrelatorQA("TrackTwoPointCorrelatorQA");
  twoCorrTrkQA -> Configure();
  f4a -> registerSubsystem(twoCorrTrkQA);

  TwoPointCorrelatorQA* twoCorrCalQA = new TwoPointCorrelatorQA("ClusterTwoPointCorrelatorQA");
  twoCorrCalQA -> Configure();
  f4a -> registerSubsystem(twoCorrCalQA);

  // run  modules and exit ----------------------------------------------------

  // run4all
  f4a -> run(nEvts);
  f4a -> End();

  // construct output file name
  const std::string outQAFile = outFileBase + "_qa.root";

  // save qa output and exit
  QAHistManagerDef::saveQARootFile(outQAFile);
  delete f4a;

  // close and exit
  gSystem -> Exit(0);
  return;

}

// end ------------------------------------------------------------------------
