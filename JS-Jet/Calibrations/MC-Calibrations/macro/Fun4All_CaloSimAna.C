#ifndef MACRO_FUN4ALL_JESMCTREEGEN_C
#define MACRO_FUN4ALL_JESMCTREEGEN_C

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
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

#include <calotrigger/CaloTriggerEmulator.h>

#include <JESMCTreeGen/JESMCTreeGen.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libclusteriso.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libffarawobjects.so)
R__LOAD_LIBRARY(libJESMCTreeGen.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)

void Fun4All_CaloSimAna(
    const int nEvents = 0,
    const string &inputFile0 = "example_list/g4hits.list",
    const string &inputFile1 = "example_list/dst_calo_waveform.list",
    const string &inputFile2 = "example_list/dst_global.list",
    const string &inputFile3 = "example_list/dst_truth_jet.list",
    const string &outputFile = "output_sim.root")
{
  // ********** Setting ********** //
  int verbosity = 0;
  bool doZVertexReadout = true;
  bool doTowerInfoReadout = false;
    bool doTowerInfoReadout_RetowerEMCal = false;
  bool doClusterReadout = false;
    bool doClusterIsoReadout = false;
    bool doClusterCNNReadout = false;
  bool doRecoJetReadout = true;
    std::vector<float> doRecoJet_radius = {0.4};
    bool doSeedJetRawReadout = false;
    bool doSeedJetSubReadout = false;
  bool doTruthJetReadout = true;
    std::vector<float> doTruthJet_radius = {0.4};
  bool doTruthParticleReadout = false;

  // ********** Initialization for Fun4All ********** //
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP", 47289);
  CDBInterface::instance()->Verbosity(1);

  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  // ********** Register subsystems ********** //
  // Cluster
  ClusterIso *cliso4;
  if (doClusterIsoReadout) {
    cliso4 = new ClusterIso("ClusterIso4",1,4,false,true);
    cliso4->Verbosity(0);
    cliso4->set_use_towerinfo(true);
    se->registerSubsystem(cliso4);
  }

  RawClusterCNNClassifier *cnn;
  if (doClusterCNNReadout) {
    cnn = new RawClusterCNNClassifier("CNN_single");
    cnn->set_modelPath("/sphenix/u/shuhang98/core_patch/coresoftware/offline/packages/CaloReco/functional_model_single.onnx");
    cnn->set_inputNodeName("CLUSTERINFO_CEMC");
    cnn->set_outputNodeName("CLUSTERINFO_CEMC_CNN");
    se->registerSubsystem(cnn);
  }

  // JetReco
  std::string jetreco_input_prefix = "TOWERINFO_CALIB";
  RetowerCEMC *_retowerCEMC;
  JetReco *_jetReco_seed;
  DetermineTowerBackground *_determineTowerBackground;
  CopyAndSubtractJets *_copyAndSubtractJets;
  DetermineTowerBackground *_determineTowerBackground2;
  SubtractTowers *_subtractTowers;
  JetReco *_jetReco;
  if (doRecoJetReadout) {
    _retowerCEMC = new RetowerCEMC();
    _retowerCEMC->Verbosity(verbosity);
    _retowerCEMC->set_towerinfo(true);
    _retowerCEMC->set_frac_cut(0.5); //fraction of retower that must be masked to mask the full retower
    _retowerCEMC->set_towerNodePrefix(jetreco_input_prefix);
    se->registerSubsystem(_retowerCEMC);
  
    _jetReco_seed = new JetReco();
    _jetReco_seed->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER,jetreco_input_prefix));
    _jetReco_seed->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO,jetreco_input_prefix));
    _jetReco_seed->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO,jetreco_input_prefix));
    _jetReco_seed->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.2), "AntiKt_TowerInfo_HIRecoSeedsRaw_r02");
    _jetReco_seed->set_algo_node("ANTIKT");
    _jetReco_seed->set_input_node("TOWER");
    _jetReco_seed->Verbosity(verbosity);
    se->registerSubsystem(_jetReco_seed);
    
    _determineTowerBackground = new DetermineTowerBackground();
    _determineTowerBackground->SetBackgroundOutputName("TowerInfoBackground_Sub1");
    _determineTowerBackground->SetFlow(false);
    _determineTowerBackground->SetSeedType(0);
    _determineTowerBackground->SetSeedJetD(3);
    _determineTowerBackground->set_towerinfo(true);
    _determineTowerBackground->Verbosity(verbosity); 
    _determineTowerBackground->set_towerNodePrefix(jetreco_input_prefix);
    se->registerSubsystem(_determineTowerBackground);
    
    _copyAndSubtractJets = new CopyAndSubtractJets();
    _copyAndSubtractJets->SetFlowModulation(false);
    _copyAndSubtractJets->Verbosity(verbosity); 
    _copyAndSubtractJets->set_towerinfo(true);
    _copyAndSubtractJets->set_towerNodePrefix(jetreco_input_prefix);
    se->registerSubsystem(_copyAndSubtractJets);
      
    _determineTowerBackground2 = new DetermineTowerBackground();
    _determineTowerBackground2->SetBackgroundOutputName("TowerInfoBackground_Sub2");
    _determineTowerBackground2->SetFlow(false);
    _determineTowerBackground2->SetSeedType(1);
    _determineTowerBackground2->SetSeedJetPt(7);
    _determineTowerBackground2->Verbosity(verbosity); 
    _determineTowerBackground2->set_towerinfo(true);
    _determineTowerBackground2->set_towerNodePrefix(jetreco_input_prefix);
    se->registerSubsystem(_determineTowerBackground2);
      
    _subtractTowers = new SubtractTowers();
    _subtractTowers->SetFlowModulation(false);
    _subtractTowers->Verbosity(verbosity);
    _subtractTowers->set_towerinfo(true);
    _subtractTowers->set_towerNodePrefix(jetreco_input_prefix);
    se->registerSubsystem(_subtractTowers);
      
    _jetReco = new JetReco();
    _jetReco->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_SUB1, jetreco_input_prefix));
    _jetReco->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO_SUB1, jetreco_input_prefix));
    _jetReco->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO_SUB1, jetreco_input_prefix));
    _jetReco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.4), "AntiKt_subtracted_r04");
    _jetReco->set_algo_node("ANTIKT");
    _jetReco->set_input_node("TOWER");
    _jetReco->Verbosity(verbosity);
    se->registerSubsystem(_jetReco);
  }

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
  _JESMCTreeGen->DoZVertexReadout(doZVertexReadout);
  _JESMCTreeGen->DoTowerInfoReadout(doTowerInfoReadout);
  _JESMCTreeGen->DoTowerInfoReadout_RetowerEMCal(doTowerInfoReadout_RetowerEMCal);
  _JESMCTreeGen->DoClusterReadout(doClusterReadout);
  _JESMCTreeGen->DoClusterIsoReadout(doClusterIsoReadout);
  _JESMCTreeGen->DoClusterCNNReadout(doClusterCNNReadout);
  _JESMCTreeGen->DoRecoJetReadout(doRecoJetReadout);
  _JESMCTreeGen->DoRecoJet_radius(doRecoJet_radius);
  _JESMCTreeGen->DoSeedJetRawReadout(doSeedJetRawReadout);
  _JESMCTreeGen->DoSeedJetSubReadout(doSeedJetSubReadout);
  _JESMCTreeGen->DoTruthJetReadout(doTruthJetReadout);
  _JESMCTreeGen->DoTruthJet_radius(doTruthJet_radius);
  _JESMCTreeGen->DoTruthParticleReadout(doTruthParticleReadout);
  se->registerSubsystem(_JESMCTreeGen);
  
  se->run(nEvents);
  CDBInterface::instance()->Print();  // print used DB files
  se->End();
  cout << "JOB COMPLETE." <<endl;
  se->PrintTimer();
  gSystem->Exit(0);
}
#endif