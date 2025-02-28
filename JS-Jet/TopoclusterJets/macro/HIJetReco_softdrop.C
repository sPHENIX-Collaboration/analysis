#ifndef MACRO_HIJETRECO_C
#define MACRO_HIJETRECO_C

#include <GlobalVariables.C>

#include <jetbase/FastJetAlgo.h>
#include <jetbase/FastJetOptions.h>
#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>
#include <jetbase/ClusterJetInput.h>
#include <g4jets/TruthJetInput.h>

#include <jetbackground/CopyAndSubtractJets.h>
#include <jetbackground/DetermineTowerBackground.h>
#include <jetbackground/FastJetAlgoSub.h>
#include <jetbackground/RetowerCEMC.h>
#include <jetbackground/SubtractTowers.h>
#include <jetbackground/SubtractTowersCS.h>

#include <fun4all/Fun4AllServer.h>

R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)

namespace Enable
{
  bool HIJETS = false;
  int HIJETS_VERBOSITY = 0;
  bool HIJETS_MC = true;
  bool HIJETS_TRUTH = false;
}  // namespace Enable

namespace HIJETS
{
  bool do_flow = false; // should be set to true once the EPD event plane correction is implemented
  bool do_CS = false;
  std::string tower_prefix = "TOWERINFO_CALIB";
}  // namespace HIJETS


void HIJetReco_softdrop()
{
  int verbosity = std::max(Enable::VERBOSITY, Enable::HIJETS_VERBOSITY);


  //---------------
  // Fun4All server
  //---------------
  Fun4AllServer *se = Fun4AllServer::instance();

  if (Enable::HIJETS_MC && Enable::HIJETS_TRUTH) 
    {
      JetReco *truthjetreco = new JetReco();
      TruthJetInput *tji = new TruthJetInput(Jet::PARTICLE);
      tji->add_embedding_flag(0);  // changes depending on signal vs. embedded
      truthjetreco->add_input(tji);
      truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.2), "AntiKt_Truth_r02");
      truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.3), "AntiKt_Truth_r03");
      truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.4), "AntiKt_Truth_r04");
      truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.5), "AntiKt_Truth_r05");
      truthjetreco->set_algo_node("ANTIKT");
      truthjetreco->set_input_node("TRUTH");
      truthjetreco->Verbosity(verbosity);
      se->registerSubsystem(truthjetreco);
    }
  
  RetowerCEMC *rcemc = new RetowerCEMC(); 
  rcemc->Verbosity(verbosity); 
  rcemc->set_towerinfo(true);
  rcemc->set_frac_cut(0.5); //fraction of retower that must be masked to mask the full retower
  rcemc->set_towerNodePrefix(HIJETS::tower_prefix);
  se->registerSubsystem(rcemc);


  JetReco *towerjetreco = new JetReco();
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER,HIJETS::tower_prefix));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO,HIJETS::tower_prefix));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO,HIJETS::tower_prefix));
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.2), "AntiKt_TowerInfo_HIRecoSeedsRaw_r02");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);

  DetermineTowerBackground *dtb = new DetermineTowerBackground();
  dtb->SetBackgroundOutputName("TowerInfoBackground_Sub1");
  dtb->SetFlow(HIJETS::do_flow);
  dtb->SetSeedType(0);
  dtb->SetSeedJetD(3);
  dtb->set_towerinfo(true);
  dtb->Verbosity(verbosity); 
  dtb->set_towerNodePrefix(HIJETS::tower_prefix);
  se->registerSubsystem(dtb);



  CopyAndSubtractJets *casj = new CopyAndSubtractJets();
  casj->SetFlowModulation(HIJETS::do_flow);
  casj->Verbosity(verbosity); 
  casj->set_towerinfo(true);
  casj->set_towerNodePrefix(HIJETS::tower_prefix);
  se->registerSubsystem(casj);
  
  
  DetermineTowerBackground *dtb2 = new DetermineTowerBackground();
  dtb2->SetBackgroundOutputName("TowerInfoBackground_Sub2");
  dtb2->SetFlow(HIJETS::do_flow);
  dtb2->SetSeedType(1);
  dtb2->SetSeedJetPt(7);
  dtb2->Verbosity(verbosity); 
  dtb2->set_towerinfo(true);
  dtb2->set_towerNodePrefix(HIJETS::tower_prefix);
  se->registerSubsystem(dtb2);
  

  SubtractTowers *st = new SubtractTowers();
  st->SetFlowModulation(HIJETS::do_flow);
  st->Verbosity(verbosity);
  st->set_towerinfo(true);
  st->set_towerNodePrefix(HIJETS::tower_prefix);
  se->registerSubsystem(st);
  
  /*
  towerjetreco = new JetReco();
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO,HIJETS::tower_prefix));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO_SUB1,HIJETS::tower_prefix));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO_SUB1,HIJETS::tower_prefix));
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.2, verbosity), "AntiKt_Tower_r02_Sub1");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.3, verbosity), "AntiKt_Tower_r03_Sub1");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.4, verbosity), "AntiKt_Tower_r04_Sub1");
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.5, verbosity), "AntiKt_Tower_r05_Sub1");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);

  JetReco *towerjetreco_softdrop = new JetReco();
  towerjetreco_softdrop->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER,HIJETS::tower_prefix));
  towerjetreco_softdrop->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO,HIJETS::tower_prefix));
  towerjetreco_softdrop->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO,HIJETS::tower_prefix));

  FastJetOptions fj_r02_opt {{ Jet::ALGO::ANTIKT, DO_SOFTDROP, SD_BETA, 0.0, SD_ZCUT, 0.1, JET_R, 0.2 }};
  towerjetreco_softdrop->add_algo(new FastJetAlgo(fj_r02_opt), "AntiKt_TowerInfo_Unsubtracted_r02");

  FastJetOptions fj_r03_opt {{ Jet::ALGO::ANTIKT, DO_SOFTDROP, SD_BETA, 0.0, SD_ZCUT, 0.1, JET_R, 0.3 }};
  towerjetreco_softdrop->add_algo(new FastJetAlgo(fj_r03_opt), "AntiKt_TowerInfo_Unsubtracted_r03");

  FastJetOptions fj_r04_opt {{ Jet::ALGO::ANTIKT, DO_SOFTDROP, SD_BETA, 0.0, SD_ZCUT, 0.1, JET_R, 0.4, VERBOSITY, 6 }};
  towerjetreco_softdrop->add_algo(new FastJetAlgo(fj_r04_opt), "AntiKt_TowerInfo_Unsubtracted_r04");

  FastJetOptions fj_r05_opt {{ Jet::ALGO::ANTIKT, DO_SOFTDROP, SD_BETA, 0.0, SD_ZCUT, 0.1, JET_R, 0.5 }};
  towerjetreco_softdrop->add_algo(new FastJetAlgo(fj_r05_opt), "AntiKt_TowerInfo_Unsubtracted_r05");

  towerjetreco_softdrop->set_algo_node("ANTIKT");
  towerjetreco_softdrop->set_input_node("TOWER");
  towerjetreco_softdrop->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco_softdrop);*/
  

  
  JetReco *topocluster_jetreco = new JetReco();
  topocluster_jetreco->add_input(new ClusterJetInput(Jet::HCAL_TOPO_CLUSTER));
  topocluster_jetreco->add_input(new ClusterJetInput(Jet::ECAL_TOPO_CLUSTER));

/*
  FastJetOptions fj_r02_opt {{ Jet::ALGO::ANTIKT, CONSTITUENT_MIN_E, 0.2, JET_R, 0.2 }};
  topocluster_jetreco->add_algo(new FastJetAlgo(fj_r02_opt), "AntiKt_TopoCluster_Unsubtracted_r02");

  FastJetOptions fj_r03_opt {{ Jet::ALGO::ANTIKT, CONSTITUENT_MIN_E, 0.2, JET_R, 0.3 }};
  topocluster_jetreco->add_algo(new FastJetAlgo(fj_r03_opt), "AntiKt_TopoCluster_Unsubtracted_r03");
*/

  FastJetOptions fj_r04_opt {{ Jet::ALGO::ANTIKT, CONSTITUENT_MIN_E, 0.2, JET_R, 0.4, VERBOSITY, 6 }};
  topocluster_jetreco->add_algo(new FastJetAlgo(fj_r04_opt), "AntiKt_TopoCluster_Unsubtracted_r04");

  //FastJetOptions fj_r05_opt {{ Jet::ALGO::ANTIKT, CONSTITUENT_MIN_E, 0.2, JET_R, 0.5 }};
  //topocluster_jetreco->add_algo(new FastJetAlgo(fj_r05_opt), "AntiKt_TopoCluster_Unsubtracted_r05");

  topocluster_jetreco->set_algo_node("ANTIKT");
  topocluster_jetreco->set_input_node("CLUSTER");
  topocluster_jetreco->Verbosity(verbosity);
  se->registerSubsystem(topocluster_jetreco);

/*
  JetReco *alllayers_jetreco = new JetReco();
  alllayers_jetreco->add_input(new ClusterJetInput(Jet::ECAL_HCAL_TOPO_CLUSTER));
  alllayers_jetreco->add_algo(new FastJetAlgo(fj_r04_opt), "AntiKt_TopoClusterAll_Unsubtracted_r04");

  alllayers_jetreco->set_algo_node("ANTIKT");
  alllayers_jetreco->set_input_node("CLUSTER");
  alllayers_jetreco->Verbosity(verbosity);
  se->registerSubsystem(alllayers_jetreco);*/

  return;

}
#endif
