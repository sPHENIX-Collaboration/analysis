#pragma once

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <GlobalVariables.C>
#include <G4_Global.C>
#include <G4_Bbc.C>

#include <pythiaemcalana/pythiaEMCalAna.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libpythiaEMCalAna.so)

void Fun4All_pythiaEMCalAna(
                     int nEvents = 1,
                     const char *filelist1 = "dst_calo_cluster.list",
                     const char *filelist2 = "dst_truth.list",
		     const string outname = "pythiaEMCalTrees.root")
{
  // this convenience library knows all our i/o objects so you don't
  // have to figure out what is in each dst type
  gSystem->Load("libg4dst.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);  // set it to 1 if you want event printouts

  Fun4AllInputManager *inCluster = new Fun4AllDstInputManager("DSTCaloCluster");
  std::cout << "Adding file list " << filelist1 << std::endl;
  inCluster->AddListFile(filelist1,1);
  se->registerInputManager(inCluster);

  Fun4AllInputManager *inTruth = new Fun4AllDstInputManager("DSTTruth");
  std::cout << "Adding file list " << filelist2 << std::endl;
  inTruth -> AddListFile(filelist2,1);
  se -> registerInputManager(inTruth);

  pythiaEMCalAna *eval = new pythiaEMCalAna(outname.c_str());
  eval -> setGenEvent(1);
  se -> registerSubsystem(eval);
  
  se->run(nEvents);
  se->End();
  
  delete se;
  cout << "Analysis Completed" << endl;
  
  gSystem->Exit(0);
}

/*{ */

  
/*  std::cout << "Starting...\n"; */
  
/*  Fun4AllServer *se = Fun4AllServer::instance(); */
/*  int verbosity = 0; */

/*  se->Verbosity(verbosity); */
/*  recoConsts *rc = recoConsts::instance(); */
/*  //rc->set_IntFlag("RUNNUMBER",62); */


/*  RetowerCEMC *rcemc = new RetowerCEMC(); */  
/*  //rcemc->SetEnergyDistribution(1); */
/*  se->registerSubsystem(rcemc); */

/*  std::cout << "jet stuff...\n"; */
/*  //seeds */
/*  JetReco *towerjetreco = new JetReco(); */
/*  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWER_RETOWER)); */
/*  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWER)); */
/*  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWER)); */
/*  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.2), "AntiKt_Tower_HIRecoSeedsRaw_r02"); */
/*  towerjetreco->set_algo_node("ANTIKT"); */
/*  towerjetreco->set_input_node("TOWER"); */
  
/*  se->registerSubsystem(towerjetreco); */
  
  

/*  std::cout << "UE subtraction stuff...\n"; */
/*  //first UE subtraction pass */
/*  DetermineTowerBackground *dtb = new DetermineTowerBackground(); */
/*  dtb->SetBackgroundOutputName("TowerBackground_Sub1"); */
/*  dtb->SetFlow(G4HIJETS::do_flow); */
/*  dtb->SetSeedType(0); */
/*  dtb->SetSeedJetD(3); */
  
/*  se->registerSubsystem(dtb); */

/*  CopyAndSubtractJets *casj = new CopyAndSubtractJets(); */
/*  casj->SetFlowModulation(G4HIJETS::do_flow); */
/*  casj->Verbosity(verbosity); */ 
/*  se->registerSubsystem(casj); */
  
 
  
/*  DetermineTowerBackground *dtb2 = new DetermineTowerBackground(); */
/*  dtb2->SetBackgroundOutputName("TowerBackground_Sub2"); */
/*  dtb2->SetFlow(G4HIJETS::do_flow); */
/*  dtb2->SetSeedType(1); */
/*  dtb2->SetSeedJetPt(7); */
/*  dtb2->Verbosity(verbosity); */ 
/*  se->registerSubsystem(dtb2); */

/*  //subtract UE from towers */
/*  SubtractTowers *st = new SubtractTowers(); */
/*  st->SetFlowModulation(G4HIJETS::do_flow); */
/*  st->Verbosity(verbosity); */
/*  se->registerSubsystem(st); */

/*  std::cout << "ClusterIso stuff...\n"; */
/*   //I think this goes here? */
/*  ClusterIso *makeClusterEt = new ClusterIso("pythiaEMCalAna",0,3,1,0); */
/*  /1* makeClusterEt -> Verbosity(0); *1/ */
/*  /1* se->registerSubsystem(makeClusterEt); *1/ */
/*  ///??? */
/*  //substract UE from jets */
/*  /1*CopyAndSubtractJets *casj = new CopyAndSubtractJets(); */
/*  casj->SetFlowModulation(G4HIJETS::do_flow); */
  
/*  se->registerSubsystem(casj); */

/*  //second pass of UE subtraction */
/*  DetermineTowerBackground *dtb2 = new DetermineTowerBackground(); */
/*  dtb2->SetBackgroundOutputName("TowerBackground_Sub2"); */
/*  dtb2->SetFlow(G4HIJETS::do_flow); */
/*  dtb2->SetSeedType(1); */
/*  dtb2->SetSeedJetPt(7); */
  
/* se->registerSubsystem(dtb2); */
  
/*  //now we build our regular jets. Not used for eT analysis right now */
/*  //but could add in function easily. */ 
/*  /1*towerjetreco = new JetReco(); */
/*  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWER_SUB1)); */
/*  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWER_SUB1)); */
/*  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWER_SUB1)); */
/*  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.2, 1), "AntiKt_Tower_r02_Sub1"); */
/*  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.3, 1), "AntiKt_Tower_r03_Sub1"); */
/*  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.4, 1), "AntiKt_Tower_r04_Sub1"); */
/*  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.5, 1), "AntiKt_Tower_r05_Sub1"); */
/*  towerjetreco->set_algo_node("ANTIKT"); */
/*  towerjetreco->set_input_node("TOWER"); */
/*  towerjetreco->Verbosity(verbosity); */
/*  se->registerSubsystem(towerjetreco); */
  
 
/*  std::cout << "more stuff...\n"; */

/*  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo"); */
/*  in->AddListFile(filelist1,1); */
/*  se->registerInputManager(in); */

/*  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTTruth"); */
/*  in2 -> AddListFile(filelist2,1); */
/*  se -> registerInputManager(in2); */
  
/*  std::cout << "pythiaEMCalAna stuff...\n"; */

/*  pythiaEMCalAna *eval = new pythiaEMCalAna(outname.c_str()); */
/*  eval -> setGenEvent(1); */
/*  se -> registerSubsystem(eval); */

/*  std::cout << "running events...\n"; */

/*  se -> run(nEvents); */
/*  std::cout << "ending...\n"; */
/*  se -> End(); */
    
/*  std::cout << "finished!\n"; */

/*} */
