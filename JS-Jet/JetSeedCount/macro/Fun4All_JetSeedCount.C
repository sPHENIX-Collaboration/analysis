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

//#include <g4jets/FastJetAlgo.h>
//#include <g4jets/JetReco.h>
//#include <g4jets/TowerJetInput.h>
//#include <g4jets/TruthJetInput.h>
#include <HIJetReco.C>
#include <g4centrality/PHG4CentralityReco.h>

//#include <jetbackground/CopyAndSubtractJets.h>
//#include <jetbackground/DetermineTowerBackground.h>
//#include <jetbackground/FastJetAlgoSub.h>
//#include <jetbackground/RetowerCEMC.h>
//#include <jetbackground/SubtractTowers.h>
//#include <jetbackground/SubtractTowersCS.h>

#include <jetvalidation/JetValidation.h>
#include <jetseedcount/JetSeedCount.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libJetSeedCount.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4dst.so)

/*
namespace Enable
{
  bool HIJETS = true;
  int HIJETS_VERBOSITY = 1;
}  // namespace Enable

namespace G4HIJETS
{
  bool do_flow = false;
  bool do_CS = false;
}  // namespace G4HIJETS
*/
#endif


void Fun4All_JetSeedCount(const char *filelisthit = "g4hit_test.list",
                     const char *filelistcalo = "dst_calo_test.list",  
                     const char *outname = "SeedCountoutput.root",
                     int n_skip = 100,
                     int n_event = 1000)
{

  
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(0);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
  se->registerSubsystem( cent );

  HIJetReco();
  /*
  RetowerCEMC *rcemc = new RetowerCEMC(); 
  rcemc->Verbosity(verbosity);
  rcemc->set_towerinfo(true); 
  se->registerSubsystem(rcemc);

  JetReco *towerjetreco = new JetReco();
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO));
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.2), "AntiKt_TowerInfo_HIRecoSeedsRaw_r02");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity); 
  se->registerSubsystem(towerjetreco);
  
  DetermineTowerBackground *dtb = new DetermineTowerBackground();
  dtb->SetBackgroundOutputName("TowerInfoBackground_Sub1");
  dtb->SetFlow(0);
  dtb->SetSeedType(0);
  dtb->SetSeedJetD(3);
  dtb->set_towerinfo(true);
  dtb->Verbosity(10); 
  se->registerSubsystem(dtb);
  
  CopyAndSubtractJets *casj = new CopyAndSubtractJets();
  casj->SetFlowModulation(0);
  casj->Verbosity(10); 
  casj->set_towerinfo(true);
  se->registerSubsystem(casj);
  
  DetermineTowerBackground *dtb2 = new DetermineTowerBackground();
  dtb2->SetBackgroundOutputName("TowerInfoBackground_Sub2");
  dtb2->SetFlow(0);
  dtb2->SetSeedType(1);
  dtb2->SetSeedJetPt(7);
  dtb2->Verbosity(10); 
  dtb2->set_towerinfo(true);
  se->registerSubsystem(dtb2);
  
  SubtractTowers *st = new SubtractTowers();
  st->SetFlowModulation(0);
  st->Verbosity(10);
  st->set_towerinfo(true);
  se->registerSubsystem(st);
  
  towerjetreco = new JetReco();
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_SUB1));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO_SUB1));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO_SUB1));
  //towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.2, 1), "AntiKt_Tower_r02_Sub1");
  //towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.3, 1), "AntiKt_Tower_r03_Sub1");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.4, 1), "AntiKt_Tower_r04_Sub1");
  //towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.5, 1), "AntiKt_Tower_r05_Sub1");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);
 
  */
  JetSeedCount *myJetVal = new JetSeedCount("AntiKt_Tower_r04_Sub1", "AntiKt_Truth_r04", outname);

  myJetVal->setPtRange(5, 100);
  myJetVal->setEtaRange(-1.1, 1.1);
  //myJetVal->doUnsub(1);
  //myJetVal->doTruth(1);
  //myJetVal->doSeeds(1);
  se->registerSubsystem(myJetVal);
  
  
  Fun4AllInputManager *intrue = new Fun4AllDstInputManager("DSThits");
  intrue->AddListFile(filelisthit,1);
  se->registerInputManager(intrue);
 

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalo");
  in2->AddListFile(filelistcalo,1);
  se->registerInputManager(in2);

  
  //se->run(-1);
  se->run(n_event);
  se->skip(n_skip);
  se->End();

  gSystem->Exit(0);
  return 0;

}
