#ifndef MACRO_FUN4ALLJETANA_C
#define MACRO_FUN4ALLJETANA_C

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>
#include <g4centrality/PHG4CentralityReco.h>
#include <g4jets/FastJetAlgo.h>
#include <g4jets/JetReco.h>
#include <g4jets/TowerJetInput.h>
#include <g4jets/TruthJetInput.h>
#include <jetbackground/FastJetAlgoSub.h>

// here you need your package name (set in configure.ac)
#include <fastjetmedianbkg/RhoMedianFluct.h>
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(librhomedianfluct.so)

void Fun4All_RhoMedianFluct(
    const string& out_name="test_RhoMedianFluct.root"
  , const int     nevnt = 10
  , const string& list_calo_cluster=""
  , const string& list_bbc=""
  , const int     verbosity=0
  )
{
  gSystem->Load("librhomedianfluct");
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
  RhoMedianFluct *rhoMedianFluct =  new RhoMedianFluct(out_name, print_stats_freq);
  rhoMedianFluct->setPtRange(5, 100);
//    rhoMedianFluct->setEtaRange(-2, 2); // this was to confirm that events without a truth lead
//          jet over 10GeV are due to the jet being outside the kinematic acceptance
  rhoMedianFluct->add_input(new TowerJetInput(Jet::CEMC_TOWER));
  rhoMedianFluct->add_input(new TowerJetInput(Jet::HCALIN_TOWER));
  rhoMedianFluct->add_input(new TowerJetInput(Jet::HCALOUT_TOWER));
  rhoMedianFluct->Verbosity(verbosity);
  se->registerSubsystem(rhoMedianFluct);

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
