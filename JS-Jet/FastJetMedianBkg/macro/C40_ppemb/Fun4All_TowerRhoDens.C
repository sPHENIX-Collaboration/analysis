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

// here you need your package name (set in configure.ac)
#include <fastjetmedianbkg/TowerRhoDens.h>
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libjetrhomedian.so)
/* R__LOAD_LIBRARY(libg4vertex.so) */

void Fun4All_TowerRhoDens(
    const string& out_name="test_TowerRhoDens.root"
  , const int     nevnt = 100
  , const string& list_calo_cluster=""
  , const string& list_truth_jet=""
  , const string& list_bbc=""
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

  int print_stats_freq = 100;
  TowerRhoDens *mod = new TowerRhoDens(out_name, print_stats_freq);
  mod->add_input(new TowerJetInput(Jet::CEMC_TOWER));
  mod->add_input(new TowerJetInput(Jet::HCALIN_TOWER));
  mod->add_input(new TowerJetInput(Jet::HCALOUT_TOWER));
  mod->Verbosity(verbosity);
  se->registerSubsystem(mod);

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

  se->run(nevnt);
  se->End();
  delete se;
  gSystem->Exit(0);
}

#endif
