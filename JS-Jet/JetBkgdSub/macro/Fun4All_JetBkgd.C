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

#include <jetbkgdsub/JetBkgdSub.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libJetBkgdSub.so)



#endif


void Fun4All_JetBkgd(
  const char *filelisttruth = "dst_truth_jet.list",
  const char *filelistcalo = "dst_calo_cluster.list",
	const char *filelistglobal = "dst_global.list",
  const char * outputfile = "output.root",
  const double jet_parameter = 0.4,
  const double min_leading_jet_pt = 30,
  const double max_leading_jet_pt = 1000  
)
{

  //-----------------------------------
  // Initialization
  //-----------------------------------

  // check jet parameter is in range 0.2-0.8
  if (jet_parameter < 0.2 || jet_parameter > 0.8)
  {
    std::cout << "Jet parameter must be in range 0.2-0.8" << std::endl;
    exit(-1);
  }
  // create jetreco and jettruth node names
  string rawname = "AntiKt_Tower_r0" + to_string(int(jet_parameter * 10));
  string truthname = "AntiKt_Truth_r0" + to_string(int(jet_parameter * 10));
  string reconame = "AntiKt_Tower_r0" + to_string(int(jet_parameter * 10))+ "_Sub1";
  // check that min leading jet pt is less than max leading jet pt
  if (min_leading_jet_pt > max_leading_jet_pt)
  {
    std::cout << "Min leading jet pt must be less than max leading jet pt" << std::endl;
    exit(-1);
  }


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

  // retower CEMC
  RetowerCEMC *rcemc = new RetowerCEMC(); 
  rcemc->Verbosity(verbosity); 
  se->registerSubsystem(rcemc);


  //-----------------------------------
  // Jet reco
  //-----------------------------------
  Enable::HIJETS_TRUTH=false;
  HIJetReco();
    
  // // tower jets
  JetReco *towerjetreco = new JetReco();
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO));
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, jet_parameter), rawname);
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  towerjetreco->Verbosity(verbosity);
  se->registerSubsystem(towerjetreco);

  // ==============
  // Jet Bkgd Sub
  // ==============
  JetBkgdSub *myJetTree = new JetBkgdSub(jet_parameter,reconame,rawname,truthname,outputfile);
  myJetTree->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER));
  myJetTree->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO));
  myJetTree->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO));
  myJetTree->setEtaRange(-1.1, 1.1);
  myJetTree->setPtRange(5, 100);
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
