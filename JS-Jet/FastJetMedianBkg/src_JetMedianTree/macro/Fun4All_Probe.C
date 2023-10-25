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
#include <fastjetmedianbkg/JetRhoMedian.h>
#include <fastjetmedianbkg/PrintTowers.h>
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libjetrhomedian.so)
R__LOAD_LIBRARY(libjetbackground.so)
/* R__LOAD_LIBRARY(libg4vertex.so) */

void Fun4All_Probe(
    const int     nevnt = 10
  , const float   jet_R = 0.4
  , const string& out_name = "test.root"
  , const float   min_lead_truth_pt = 30
  , const string& list_calo_cluster=""
  , const string& list_truth_jet=""
  , const string& list_bbc=""
  , const string& list_global=""
  , const string& list_truth_g4hit=""
  , const int     verbosity=0
  )
{
  gSystem->Load("libjetrhomedian");
  gSystem->Load("libg4dst");

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

  //
  // do the background subtraction

  /* std::string sub1_jet_name = Form("AntiKt_Tower_r0%i_Sub1", ((int)(jet_R*10))); */
  std::string truth_jet_name = Form("AntiKt_Truth_r0%i", ((int)(jet_R*10)));


  if (list_truth_jet!="") {
    Fun4AllInputManager *inp_truth_jet = new Fun4AllDstInputManager("DSTtruth");
    inp_truth_jet->AddListFile(list_truth_jet,1); // adding the option "1" confirms to use, even if file is large
    se->registerInputManager(inp_truth_jet);
  } else {
    cout << " Fatal: missing truth jet list file. Exiting." << endl;
    gSystem->Exit(0);
  }

  if (list_calo_cluster!="") {
    Fun4AllInputManager *inp_calo_cluster = new Fun4AllDstInputManager("DSTcalocluster");
    inp_calo_cluster->AddListFile(list_calo_cluster,1);
    se->registerInputManager(inp_calo_cluster);
  } else {
    cout << " Fatal: missing calo cluster list file. Exiting." << endl;
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

  if (list_truth_g4hit!="") {
    Fun4AllInputManager *inp_truth_g4hit = new Fun4AllDstInputManager("DST_truth_g4hit");
    inp_truth_g4hit->AddListFile(list_truth_g4hit,1); // adding the option "1" confirms to use, even if file is large
    se->registerInputManager(inp_truth_g4hit);
  } else {
    cout << " Fatal: missing truth g4hit list file. Exiting." << endl;
    gSystem->Exit(0);
  }
 
  // add the CEMC retowering
  RetowerCEMC *rcemc = new RetowerCEMC(); 
  /* rcemc->Verbosity(verbosity); */ 
  rcemc->set_towerinfo(true);
  se->registerSubsystem(rcemc);

  Enable::HIJETS_TRUTH=false;
  HIJetReco();

    /* auto towerinfo_jets = new JetReco(); */
    /* towerinfo_jets->add_input(new TowerJetInput(Jet::CEMC_TOWER_RETOWER)); */
    /* towerinfo_jets->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO)); */
    /* towerinfo_jets->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO)); */
    /* towerinfo_jets->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.4), "TOWERINFO_AntiKt_Tower_r04"); */
    /* towerinfo_jets->set_algo_node("ANTIKT"); */
    /* towerinfo_jets->set_input_node("TOWER"); */
    /* se->registerSubsystem(towerinfo_jets); */

    /* auto SUB1_jets = new JetReco(); */
    /* SUB1_jets->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_SUB1)); */
    /* SUB1_jets->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO_SUB1)); */
    /* SUB1_jets->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO_SUB1)); */
    /* SUB1_jets->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.4), "SUB1_AntiKt_Tower_r04"); */
    /* SUB1_jets->set_algo_node("ANTIKT"); */
    /* SUB1_jets->set_input_node("TOWER"); */
    /* se->registerSubsystem(SUB1_jets); */

  PrintTowers* print_tow = new PrintTowers( {{truth_jet_name
      /* , "SUB1_AntiKt_Tower_r04" */
      , "AntiKt_Tower_r04_Sub1"
      }}, { {{truth_jet_name, "AntiKt_Tower_r04_Sub1"} }}, {{
      {Jet::CEMC_TOWER_RETOWER, "CEMC_TOWER_RETOWER"},
      {Jet::HCALIN_TOWER,  "HCALIN_TOWER"},
      {Jet::HCALIN_TOWER,  "HCALOUT_TOWER"},
      {Jet::CEMC_TOWERINFO_RETOWER, "CEMC_TOWERINFO_RETOWER"},
      {Jet::HCALIN_TOWERINFO,  "HCALIN_TOWERINFO"},
      {Jet::HCALOUT_TOWERINFO, "HCALOUT_TOWERINFO"},
      {Jet::CEMC_TOWERINFO_SUB1,    "CEMC_TOWERSUB1"},
      {Jet::HCALIN_TOWERINFO_SUB1, "HCALIN_TOWERINFO_SUB1"},
      {Jet::HCALOUT_TOWERINFO_SUB1, "HCALOUT_TOWERINFO_SUB1"}
}}, 5, 0.3 );
  /* PrintTowers* print_tow = new PrintTowers( {{truth_jet_name, "TOWERINFO_AntiKt_Tower_r04", "SUB1_AntiKt_Tower_r04" }}, {}, 10., 10); */
  print_tow->nmax_jetprint = 20;
  /* print_tow->m_max_cent = 10; */
      /* {Jet::CEMC_TOWER_RETOWER, "CEMC_TOWER_RETOWER"}, */
      /* {Jet::CEMC_TOWERINFO_RETOWER, "CEMC_TOWERINFO_RETOWER"}, */
      /* {Jet::HCALIN_TOWERINFO,  "HCALIN_TOWERINFO"}, */
      /* {Jet::HCALOUT_TOWERINFO, "HCALOUT_TOWERINFO"}, */
  /* }}, 5., 0.01 ); */

  /* PrintTowers* print_tow = new PrintTowers( {{truth_jet_name, sub1_jet_name, "TOWERINFO_AntiKt_Tower_r04", "SUB1_AntiKt_Tower_r04"}}, {{ */ 
  /*     {Jet::CEMC_TOWERINFO_RETOWER, "CEMC_TOWERINFO_RETOWER"}, */
  /*     {Jet::HCALIN_TOWERINFO,  "HCALIN_TOWERINFO"}, */
  /*     {Jet::HCALOUT_TOWERINFO, "HCALOUT_TOWERINFO"}, */
  /*     {Jet::CEMC_TOWERINFO_SUB1,    "CEMC_TOWERSUB1"}, */
  /*     {Jet::HCALIN_TOWERINFO_SUB1, "HCALIN_TOWERINFO_SUB1"}, */
  /*     {Jet::HCALOUT_TOWERINFO_SUB1, "HCALOUT_TOWERINFO_SUB1"} */
  /* }}, 5., 0.10 ); */
  se->registerSubsystem(print_tow);

  se->run(nevnt);
  se->End();
  delete se;
  cout << " Done in Fun4All_Probe.C " << endl;
  gSystem->Exit(0);

}

#endif
