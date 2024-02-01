#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <g4centrality/PHG4CentralityReco.h>

#include <jetbackground/RetowerCEMC.h>
#include <jetbackground/FastJetAlgoSub.h>

#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>
#include <jetbase/FastJetAlgo.h>
#include <g4jets/TruthJetInput.h>

#include <HIJetReco.C>

#include <randomconeana/RandomConeAna.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libRandomConeAna.so)



#endif


void Fun4All(
  const char *filelisttruth = "dst_truth_jet.list",
  const char *filelistcalo = "dst_calo_cluster.list",
	const char *filelistglobal = "dst_global.list",
  const char * outputfile = "output.root"
)
{

  //-----------------------------------
  // Fun4All server initialization
  //-----------------------------------

  // create fun4all server
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;
  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  // centrality
  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(0);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
  se->registerSubsystem( cent );

  //-----------------------------------
  // Jet reco
  //-----------------------------------
  Enable::HIJETS_TRUTH=false;
  HIJetReco();
    
  //-----------------------------------
  // Random Cone Reco
  //-----------------------------------
  
  RandomConeAna *myJetTree = new RandomConeAna(outputfile);
  myJetTree->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER));
  myJetTree->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO));
  myJetTree->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO));
  myJetTree->add_iter_input(new TowerJetInput(Jet::CEMC_TOWERINFO_SUB1));
  myJetTree->add_iter_input(new TowerJetInput(Jet::HCALIN_TOWERINFO_SUB1));
  myJetTree->add_iter_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO_SUB1));
  myJetTree->set_user_seed(42);
  myJetTree->setEventSelection(30,1000);
  myJetTree->addWeight(2.178e-9);
  myJetTree->Verbosity(verbosity);
  se->registerSubsystem(myJetTree);


  //-----------------------------------
  // Input managers
  //-----------------------------------

  Fun4AllInputManager *intrue = new Fun4AllDstInputManager("DSTtruth");
  intrue->AddListFile(filelisttruth,1);
  se->registerInputManager(intrue);

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalo");
  in2->AddListFile(filelistcalo,1);
  se->registerInputManager(in2);

  Fun4AllInputManager *in3 = new Fun4AllDstInputManager("DSTglobal");
  in3->AddListFile(filelistglobal,1);
  se->registerInputManager(in3);

  //-----------------------------------
  // Run the analysis
  //-----------------------------------
  
  se->run(-1);
  se->End();

  gSystem->Exit(0);
  return 0;

}
