#ifndef MACRO_FUN4ALLJETPLUSBG_C
#define MACRO_FUN4ALLJETPLUSBG_C

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>

/* #include <GlobalVariables.C> */
/* #include <G4_Global.C> */
#include <g4jets/FastJetAlgo.h>
#include <g4jets/JetReco.h>
#include <g4jets/TowerJetInput.h>
#include <g4jets/TruthJetInput.h>

#include <g4centrality/PHG4CentralityReco.h>

#include <jetbackground/FastJetAlgoSub.h>

// here you need your package name (set in configure.ac)
#include <calojetrhoest/JetPlusBackground.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libjetplusbackground.so)

void Fun4All_JetPlusBackground_forCondor(
    const double min_calo_pt=0.2,
    const int nevnt = 10,
    const int n_print_freq            = 1,
    const std::string &recojetname    = "AntiKt_Tower_r04",
    const char *index = NULL
  )
{

  const int verbosity=1;
    
  string incaloList = "lists/jet_bg/dst_calo_g4hit.list";
  string incaloclusterList = "lists/jet_bg/dst_calo_cluster.list";
  string invertexList = "lists/jet_bg/dst_vertex.list";
  string inbbcList = "lists/jet_bg/dst_bbc_g4hit.list";
  string fout_name="out/jet_bg/JetPlusBackground.root";
  

  if (index) {
    incaloList.insert(incaloList.find_first_of("."),index,4);
    incaloclusterList.insert(incaloclusterList.find_first_of("."),index,4);
    invertexList.insert(invertexList.find_first_of("."),index,4);
    inbbcList.insert(inbbcList.find_first_of("."),index,4);
    fout_name.insert(fout_name.find_first_of("."),index,4);
  }
    
  gSystem->Load("libjetplusbackground");
  gSystem->Load("libg4dst");

  Fun4AllServer *se = Fun4AllServer::instance();
    
  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(0);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
  se->registerSubsystem( cent );

  // change lower pt and eta cut to make them visible using the example
  //  pythia8 file
  int print_stats_freq = n_print_freq;
  JetPlusBackground *myJetAnalysis = new JetPlusBackground(min_calo_pt, nevnt, print_stats_freq, "AntiKt_Tower_r04", fout_name);
  myJetAnalysis->setPtRange(5, 100);
  myJetAnalysis->setEtaRange(-1.1, 1.1);
  myJetAnalysis->add_input(new TowerJetInput(Jet::CEMC_TOWER));
  myJetAnalysis->add_input(new TowerJetInput(Jet::HCALIN_TOWER));
  myJetAnalysis->add_input(new TowerJetInput(Jet::HCALOUT_TOWER));
  se->registerSubsystem(myJetAnalysis);

  Fun4AllInputManager *incalo = new Fun4AllDstInputManager("DSTcalo");
  incalo->AddListFile(incaloList,1);
  se->registerInputManager(incalo);

  Fun4AllInputManager *incalocluster = new Fun4AllDstInputManager("DSTcalocluster");
  incalocluster->AddListFile(incaloclusterList,1);
  se->registerInputManager(incalocluster);

  Fun4AllInputManager *invertex = new Fun4AllDstInputManager("DSTvertex");
  invertex->AddListFile(invertexList,1);
  se->registerInputManager(invertex);
    
  Fun4AllInputManager *inbbc = new Fun4AllDstInputManager("DSTbbc");
  inbbc->AddListFile(inbbcList,1);
  se->registerInputManager(inbbc);

  myJetAnalysis->Verbosity(verbosity);
  se->run(nevnt);
  se->End();
  delete se;
  gSystem->Exit(0);
}

#endif
