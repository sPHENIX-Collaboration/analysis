#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <jetbackground/DetermineTowerRho.h>

#include <jetbase/TowerJetInput.h>

#include <towerrhoana/TowerRhoAna.h>


#include <HIJetReco.C>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libg4dst.so)

R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libTowerRhoAna.so)

#endif

void Fun4All_jetQA(const int nEvents = -1,
		   const char *filelistcalo = "dst_calo_cluster.list",
		   const char *filelistglobal = "dst_global.list",
		   const char *outname = "test.root")
{

  //make the fun4all server
  Fun4AllServer *se = Fun4AllServer::instance();

  //set the verbosity (increase to print more output for debugging)
  int verbosity = 0;
  se->Verbosity(verbosity);
  Enable::VERBOSITY = verbosity;

  //run the jet reconstruction
  HIJetReco();

  std::vector<TowerJetInput*> background_inputs = 
  {
    new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER),
    new TowerJetInput(Jet::HCALIN_TOWERINFO),
    new TowerJetInput(Jet::HCALOUT_TOWERINFO)
  };  


  DetermineTowerRho *dtr = new DetermineTowerRho();
  dtr->add_method(TowerRho::Method::AREA); // do area method
  dtr->add_method(TowerRho::Method::MULT); // do multiplicy method
  dtr->set_algo(Jet::ALGO::KT); // default = KT
  dtr->set_par(0.4); // default = 0.4
  dtr->set_tower_abs_eta(1.1); // default=1.1
  dtr->set_jet_abs_eta(0.7); //default is tower_abs_eta - R
  dtr->set_omit_nhardest(2); // default=2
  dtr->set_ghost_area(0.01); // default=0.01
  dtr->set_jet_min_pT(0.0); // default=0.0
  for(auto input : background_inputs) dtr->add_tower_input(input); // add inputs to estimate rho
  dtr->Verbosity(verbosity);
  se->registerSubsystem(dtr); 


  // TowerRhoAna
  TowerRhoAna *ana = new TowerRhoAna(outname);
  ana->Verbosity(verbosity);
  ana->add_area_rho_node("TowerRho_AREA"); // add rho node to output tree
  ana->add_mult_rho_node("TowerRho_MULT"); // add rho node to output tree
  se->registerSubsystem(ana);

  //add inputs
  //add any additional needed inputs here
  Fun4AllInputManager *incalo = new Fun4AllDstInputManager("DSTcalo");
  incalo->AddListFile(filelistcalo,1);
  se->registerInputManager(incalo);

  Fun4AllInputManager *inglobal = new Fun4AllDstInputManager("DSTglobal");
  inglobal->AddListFile(filelistglobal,1);
  se->registerInputManager(inglobal);

  //run
  se->run(nEvents);
  se->End();

  gSystem->Exit(0);
  return 0;
}
