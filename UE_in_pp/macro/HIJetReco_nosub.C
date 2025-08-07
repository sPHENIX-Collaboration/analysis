#ifndef MACRO_HIJETRECO_C
#define MACRO_HIJETRECO_C

#include <GlobalVariables.C>

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

#include <fun4all/Fun4AllServer.h>

R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libglobalvertex.so)

namespace Enable
{
  bool HIJETS = false;
  int HIJETS_VERBOSITY = 0;
  bool HIJETS_MC = true;
  bool HIJETS_TRUTH = true;
}  // namespace Enable

namespace HIJETS
{
  bool do_flow = false; // should be set to true once the EPD event plane correction is implemented
  bool do_CS = false;
  bool is_pp = true;
  std::string tower_prefix = "TOWERINFO_CALIB";
  bool do_vertex_type = true;
  GlobalVertex::VTXTYPE vertex_type = GlobalVertex::MBD;
}  // namespace HIJETS


void HIJetReco()
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
      //truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.2), "AntiKt_Truth_r02");
      //truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.3), "AntiKt_Truth_r03");
      truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.4), "AntiKt_Truth_r04");
      //truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.5), "AntiKt_Truth_r05");
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
  TowerJetInput *incemc = new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER,HIJETS::tower_prefix);
  TowerJetInput *inihcal = new TowerJetInput(Jet::HCALIN_TOWERINFO,HIJETS::tower_prefix);
  TowerJetInput *inohcal = new TowerJetInput(Jet::HCALOUT_TOWERINFO,HIJETS::tower_prefix);
  if (HIJETS::do_vertex_type)
  {
    incemc->set_GlobalVertexType(HIJETS::vertex_type);
    inihcal->set_GlobalVertexType(HIJETS::vertex_type);
    inohcal->set_GlobalVertexType(HIJETS::vertex_type);
  }
  towerjetreco->add_input(incemc);
  towerjetreco->add_input(inihcal);
  towerjetreco->add_input(inohcal);
  towerjetreco->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.4), "AntiKt_Tower_r04");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);


  return;

}
#endif
