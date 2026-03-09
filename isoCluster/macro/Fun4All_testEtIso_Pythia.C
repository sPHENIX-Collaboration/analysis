#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <g4jets/FastJetAlgo.h>
#include <g4jets/JetReco.h>
#include <g4jets/TowerJetInput.h>
#include <g4jets/TruthJetInput.h>

#include <jetbackground/CopyAndSubtractJets.h>
#include <jetbackground/DetermineTowerBackground.h>
#include <jetbackground/FastJetAlgoSub.h>
#include <jetbackground/RetowerCEMC.h>
#include <jetbackground/SubtractTowers.h>
#include <jetbackground/SubtractTowersCS.h>

#include <ClusterIso.h>

#include <isocluster/isoCluster.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libisoCluster.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libclusteriso.so)

namespace Enable
{
  bool HIJETS = false;
  int HIJETS_VERBOSITY = 1;
}  // namespace Enable

namespace G4HIJETS
{
  bool do_flow = true;
  bool do_CS = false;
}  // namespace G4HIJETS
#endif


void Fun4All_testEtIso_Pythia(
                     int nEvents = 1,
                     const char *filelist1 = "dst_calo_cluster_run40_PhotonJet.list",
                     const char *filelist2 = "dst_truth_run40_PhotonJet.list",
		     const string outname = "test.root")
{

  
  
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();
  //rc->set_IntFlag("RUNNUMBER",62);


  RetowerCEMC *rcemc = new RetowerCEMC();  
  //rcemc->SetEnergyDistribution(1);
  se->registerSubsystem(rcemc);

  //seeds
  JetReco *towerjetreco = new JetReco();
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWER_RETOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWER));
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.2), "AntiKt_Tower_HIRecoSeedsRaw_r02");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  
  se->registerSubsystem(towerjetreco);
  
  

  //first UE subtraction pass
  DetermineTowerBackground *dtb = new DetermineTowerBackground();
  dtb->SetBackgroundOutputName("TowerBackground_Sub1");
  dtb->SetFlow(G4HIJETS::do_flow);
  dtb->SetSeedType(0);
  dtb->SetSeedJetD(3);
  
  se->registerSubsystem(dtb);

  CopyAndSubtractJets *casj = new CopyAndSubtractJets();
  casj->SetFlowModulation(G4HIJETS::do_flow);
  casj->Verbosity(verbosity); 
  se->registerSubsystem(casj);
  
 
  
  DetermineTowerBackground *dtb2 = new DetermineTowerBackground();
  dtb2->SetBackgroundOutputName("TowerBackground_Sub2");
  dtb2->SetFlow(G4HIJETS::do_flow);
  dtb2->SetSeedType(1);
  dtb2->SetSeedJetPt(7);
  dtb2->Verbosity(verbosity); 
  se->registerSubsystem(dtb2);

  //subtract UE from towers
  SubtractTowers *st = new SubtractTowers();
  st->SetFlowModulation(G4HIJETS::do_flow);
  st->Verbosity(verbosity);
  se->registerSubsystem(st);

   //I think this goes here?
  ClusterIso *makeClusterEt = new ClusterIso("isoCluster",0,3,1,0);
  makeClusterEt -> Verbosity(0);
  se->registerSubsystem(makeClusterEt);
  ///???
  //substract UE from jets
  /*CopyAndSubtractJets *casj = new CopyAndSubtractJets();
  casj->SetFlowModulation(G4HIJETS::do_flow);
  
  se->registerSubsystem(casj);

  //second pass of UE subtraction
  DetermineTowerBackground *dtb2 = new DetermineTowerBackground();
  dtb2->SetBackgroundOutputName("TowerBackground_Sub2");
  dtb2->SetFlow(G4HIJETS::do_flow);
  dtb2->SetSeedType(1);
  dtb2->SetSeedJetPt(7);
  
  se->registerSubsystem(dtb2);*/
  
  //now we build our regular jets. Not used for eT analysis right now
  //but could add in function easily. 
  /*towerjetreco = new JetReco();
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWER_SUB1));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWER_SUB1));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWER_SUB1));
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.2, 1), "AntiKt_Tower_r02_Sub1");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.3, 1), "AntiKt_Tower_r03_Sub1");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.4, 1), "AntiKt_Tower_r04_Sub1");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.5, 1), "AntiKt_Tower_r05_Sub1");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);*/
  
 
  
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  in->AddListFile(filelist1,1);
  se->registerInputManager(in);

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTTruth");
  in2 -> AddListFile(filelist2,1);
  se -> registerInputManager(in2);
  
  isoCluster *eval = new isoCluster(outname.c_str());
  eval -> setGenEvent(1);
  se -> registerSubsystem(eval);

  se -> run(nEvents);
  se -> End();
    
}
