#ifndef MACRO_FUN4ALLJETANA_C
#define MACRO_FUN4ALLJETANA_C

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>
#include <g4centrality/PHG4CentralityReco.h>
#include <jetbase/FastJetAlgo.h>
#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>
#include <g4jets/TruthJetInput.h>
/* #include <g4vertex/GlobalVertexReco.h> */
#include <jetbackground/FastJetAlgoSub.h>
#include <fun4all/PHTFileServer.h>
#include <jetbackground/RetowerCEMC.h>
#include <HIJetReco.C>

// here you need your package name (set in configure.ac)
#include <fastjetmedianbkg/RhoFluct.h>
#include <fastjetmedianbkg/PrintTowers.h>
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libjetrhomedian.so)
R__LOAD_LIBRARY(libjetbackground.so)
/* R__LOAD_LIBRARY(libg4vertex.so) */

void Fun4All_RhoFluct(
    const int     nevnt = 10
  , const float   jet_R = 0.4
  , const string& out_name = "test.root"
  /* , const float   min_lead_truth_pt = 30 */
  , const string& list_calo_cluster=""
  /* , const string& list_truth_jet="" */
  , const string& list_bbc=""
  , const string& list_global=""
  /* , const string& list_truth_g4hit="" */
  , const int     verbosity=0
  )
{
  gSystem->Load("libjetrhomedian");
  gSystem->Load("libg4dst");

  /* Enable::BBC = true; */

  Fun4AllServer *se = Fun4AllServer::instance();

  if (list_bbc!="") {
    PHG4CentralityReco *cent = new PHG4CentralityReco();
    cent->Verbosity(0);
    cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
    se->registerSubsystem( cent );

    Fun4AllInputManager *inp_bbc = new Fun4AllDstInputManager("BBC_info");
    inp_bbc->AddListFile(list_bbc);
    se->registerInputManager(inp_bbc);
  } else {
    cout << " Fatal: missing bbc input file. Exiting." << endl;
    gSystem->Exit(0);
  }

  if (list_global!="") {
    Fun4AllInputManager *inp_global = new Fun4AllDstInputManager("DSTglobal");
    inp_global->AddListFile(list_global,1); // adding the option "1" confirms to use, even if file is large
    se->registerInputManager(inp_global);
  } else {
    cout << " Fatal: missing global dst list file. Exiting." << endl;
    gSystem->Exit(0);
  }

  // add the CEMC retowering
  RetowerCEMC *rcemc = new RetowerCEMC(); 
  /* rcemc->Verbosity(verbosity); */ 
  /* rcemc->set_towerinfo(true); */
  se->registerSubsystem(rcemc);
  //
  // do the background subtraction
  Enable::HIJETS_TRUTH=false;
  HIJetReco();

  RhoFluct *rhofluct = new RhoFluct(out_name, jet_R);
  rhofluct->add_input_rhoA(new TowerJetInput(Jet::CEMC_TOWER_RETOWER));
  rhofluct->add_input_rhoA(new TowerJetInput(Jet::HCALIN_TOWER));
  rhofluct->add_input_rhoA(new TowerJetInput(Jet::HCALOUT_TOWER));
  rhofluct->add_input_Sub1(new TowerJetInput(Jet::CEMC_TOWERINFO_SUB1));
  rhofluct->add_input_Sub1(new TowerJetInput(Jet::HCALIN_TOWERINFO_SUB1));
  rhofluct->add_input_Sub1(new TowerJetInput(Jet::HCALOUT_TOWERINFO_SUB1));
  rhofluct->Verbosity(0);
  se->registerSubsystem(rhofluct);


  if (list_calo_cluster!="") {
    Fun4AllInputManager *inp_calo_cluster = new Fun4AllDstInputManager("DSTcalocluster");
    inp_calo_cluster->AddListFile(list_calo_cluster,1);
    se->registerInputManager(inp_calo_cluster);
  } else {
    cout << " Fatal: missing calo cluster list file. Exiting." << endl;
    gSystem->Exit(0);
  }

  se->run(nevnt);
  se->End();
  delete se;
  cout << " Done in Fun4All_RhoFluct.C " << endl;
  gSystem->Exit(0);

}

#endif
