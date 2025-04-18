#ifndef MACRO_FUN4ALL_JETSPECTRUMSIM_C
#define MACRO_FUN4ALL_JETSPECTRUMSIM_C

#include <mbd/MbdReco.h>
#include <GlobalVariables.C>
#include <G4_Global.C>
#include <globalvertex/GlobalVertexReco.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>

#include <phool/recoConsts.h>
#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>
#include <G4_Input.C>
#include <GlobalVariables.C>

#include <caloreco/RawClusterCNNClassifier.h>
#include <clusteriso/ClusterIso.h>

#include <jetbase/FastJetAlgo.h>
#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>
#include <g4jets/TruthJetInput.h>

#include <jetbackground/CopyAndSubtractJets.h>
#include <jetbackground/DetermineTowerBackground.h>
#include <jetbackground/FastJetAlgoSub.h>
#include <jetbackground/RetowerCEMC.h>
#include <jetbackground/SubtractTowers.h>
#include <jetbackground/SubtractTowersCS.h>

#include <globalvertex/GlobalVertex.h>

#include <calotrigger/CaloTriggerEmulator.h>

#include <JESMCTreeGen/JESMCTreeGen.h>
#include </sphenix/user/hanpuj/JES_MC_Calibration/macro/Calo_Calib.C>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libclusteriso.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libffarawobjects.so)
R__LOAD_LIBRARY(libJESMCTreeGen.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libmbd.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libglobalvertex.so)

void Fun4All_JetSpectrumSim(
    const int nEvents = 0,
    const string &inputFile0 = "example_list/g4hits.list",
    const string &inputFile1 = "example_list/dst_calo_cluster.list",
    const string &inputFile2 = "example_list/dst_truth_jet.list",
    const string &inputFile3 = "example_list/dst_mbd_epd.list",
    const string &outputFile = "output_sim.root")
{
  // ********** Setting ********** //
  int verbosity = 0;

  GlobalVertex::VTXTYPE vertex_type = GlobalVertex::MBD;

  // ********** Initialization for Fun4All ********** //
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", "MDC2");
  rc->set_uint64Flag("TIMESTAMP", 21);
  CDBInterface::instance()->Verbosity(1);

  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  // ********** Process Calo Calibration ********** //
  Process_Calo_Calib();

  MbdReco *mbdreco = new MbdReco();
  se->registerSubsystem(mbdreco);

  GlobalVertexReco *gvertex = new GlobalVertexReco();
  se->registerSubsystem(gvertex);

  // ********** Register subsystems ********** //
  // JetReco
  std::string jetreco_input_prefix = "TOWERINFO_CALIB";
  RetowerCEMC *_retowerCEMC;
  _retowerCEMC = new RetowerCEMC();
  _retowerCEMC->Verbosity(verbosity);
  _retowerCEMC->set_towerinfo(true);
  _retowerCEMC->set_frac_cut(0.5); //fraction of retower that must be masked to mask the full retower
  _retowerCEMC->set_towerNodePrefix(jetreco_input_prefix);
  se->registerSubsystem(_retowerCEMC);

  JetReco *_jetRecoUnsub;
  _jetRecoUnsub = new JetReco();
  TowerJetInput *incemc3 = new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER, jetreco_input_prefix);
  TowerJetInput *inihcal3 = new TowerJetInput(Jet::HCALIN_TOWERINFO, jetreco_input_prefix);
  TowerJetInput *inohcal3 = new TowerJetInput(Jet::HCALOUT_TOWERINFO, jetreco_input_prefix);
  incemc3->set_GlobalVertexType(vertex_type);
  inihcal3->set_GlobalVertexType(vertex_type);
  inohcal3->set_GlobalVertexType(vertex_type);
  _jetRecoUnsub->add_input(incemc3);
  _jetRecoUnsub->add_input(inihcal3);
  _jetRecoUnsub->add_input(inohcal3);
  _jetRecoUnsub->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.4), "AntiKt_unsubtracted_r04");
  _jetRecoUnsub->set_algo_node("ANTIKT");
  _jetRecoUnsub->set_input_node("TOWER");
  _jetRecoUnsub->Verbosity(verbosity);
  se->registerSubsystem(_jetRecoUnsub);

  // ********** Register input managers ********** //
  Fun4AllInputManager *in0 = new Fun4AllDstInputManager("in0");
  in0->AddListFile(inputFile0,1);
  se->registerInputManager(in0);

  Fun4AllInputManager *in1 = new Fun4AllDstInputManager("in1");
  in1->AddListFile(inputFile1,1);
  se->registerInputManager(in1);

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("in2");
  in2->AddListFile(inputFile2,1);
  se->registerInputManager(in2);

  Fun4AllInputManager *in3 = new Fun4AllDstInputManager("in3");
  in3->AddListFile(inputFile3,1);
  se->registerInputManager(in3);

  // ********** Register output managers ********** //
  JESMCTreeGen *_JESMCTreeGen = new JESMCTreeGen("JESMCTreeGen",outputFile);
  _JESMCTreeGen->SetVerbosity(verbosity);
  se->registerSubsystem(_JESMCTreeGen);
  
  se->run(nEvents);
  CDBInterface::instance()->Print();  // print used DB files
  se->End();
  cout << "JOB COMPLETE." <<endl;
  se->PrintTimer();
  gSystem->Exit(0);
}
#endif
