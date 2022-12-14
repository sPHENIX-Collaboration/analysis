#ifndef MACRO_FUN4ALLJETANA_C
#define MACRO_FUN4ALLJETANA_C

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
#include <calojetrhoest/CaloJetRhoEst.h>
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libcalojetrhoest.so)

void GenerateIONames ( string &intrueList, string &incaloList, string &incaloclusterList, string &invertexList, string &inbbcList, string &fout_name, const char *index, double min_jet_pt, bool embed ) {
    intrueList = "lists/";
    incaloList = "lists/";
    incaloclusterList = "lists/";
    invertexList = "lists/";
    inbbcList = "lists/";

    if (embed) {
      cout<<"USING EMBEDDING"<<endl;
      intrueList += "calo_rho/";
      incaloList += "calo_rho/";
      incaloclusterList += "calo_rho/";
      invertexList += "calo_rho/";
      inbbcList += "calo_rho/";
        if (min_jet_pt==10.) {fout_name = "out/calo_rho/10GeV/CaloJetRho.root";}
        else if (min_jet_pt==30.) {fout_name = "out/calo_rho/30GeV/CaloJetRho.root";}
        else { cout<<"MINIMUM JET PT MUST BE 10 OR 30"<<endl; }
    }
    else {
      cout<<"NO EMBEDDING"<<endl;
      intrueList += "no_embed/";
      incaloList += "no_embed/";
      incaloclusterList += "no_embed/";
      invertexList += "no_embed/";
      inbbcList += "no_embed/";
      if (min_jet_pt==10.) {fout_name = "out/no_embed/10GeV/CaloJetRho.root";}
      else if (min_jet_pt==30.) {fout_name = "out/no_embed/30GeV/CaloJetRho.root";}
      else { cout<<"MINIMUM JET PT MUST BE 10 OR 30"<<endl; }
    }
    
    if (min_jet_pt==10.) {
      cout<<"JETS >= 10 GeV"<<endl;
      intrueList += "10GeV/";
      incaloList += "10GeV/";
      incaloclusterList += "10GeV/";
      invertexList += "10GeV/";
      inbbcList += "10GeV/";
//      fout_name += "10GeV/";
    }
    else if (min_jet_pt==30.) {
        cout<<"JETS >= 30 GeV"<<endl;
        intrueList += "30GeV/";
        incaloList += "30GeV/";
        incaloclusterList += "30GeV/";
        invertexList += "30GeV/";
        inbbcList += "30GeV/";
//        fout_name += "30GeV/";
      }
    else {
      cout<<"MINIMUM JET PT MUST BE 10 OR 30"<<endl;
    }
    
    intrueList += "dst_truth_jet.list";
    incaloList += "dst_calo_g4hit.list";
    incaloclusterList += "dst_calo_cluster.list";
    invertexList += "dst_vertex.list";
    inbbcList += "dst_bbc_g4hit.list";
//    fout_name += "CaloJetRho.root";
    
  if (index) {
    intrueList.insert(intrueList.find_first_of("."),index,4);
    incaloList.insert(incaloList.find_first_of("."),index,4);
    incaloclusterList.insert(incaloclusterList.find_first_of("."),index,4);
    invertexList.insert(invertexList.find_first_of("."),index,4);
    inbbcList.insert(inbbcList.find_first_of("."),index,4);
    fout_name.insert(fout_name.find_first_of("."),index,4);
  }
    
//  cout<< intrueList<<"\t"<<incaloList<<"\t"<<incaloclusterList<<"\t"<<invertexList<<"\t"<<inbbcList<<"\t"<<fout_name <<endl;
};





void Fun4All_CaloJetRho_forCondor(
    const int nevnt = 100,
    const double min_calo_pt=0.2,
    const double min_jet_pt = 10.,
    const int verbosity=1,
    const char *index = NULL,
    const bool embed = false
  )
{
    
  string intrueList, incaloList, incaloclusterList, invertexList, inbbcList, fout_name;
  GenerateIONames( intrueList, incaloList, incaloclusterList, invertexList, inbbcList, fout_name, index, min_jet_pt, embed );
  cout<< intrueList<<"\t"<<incaloList<<"\t"<<incaloclusterList<<"\t"<<invertexList<<"\t"<<inbbcList<<"\t"<<fout_name <<endl;

  gSystem->Load("libcalojetrhoest");
  gSystem->Load("libg4dst");

  Fun4AllServer *se = Fun4AllServer::instance();

//  if (false) {
//    JetReco *towerjetreco = new JetReco();
//    towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWER));
//    towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWER));
//    towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWER));
//    towerjetreco->add_algo(new  FastJetAlgoSub(Jet::ANTIKT, 0.4, 1), "AntiKt_Tower_r04");
//    towerjetreco->set_algo_node("ANTIKT");
//    towerjetreco->set_input_node("TOWER");
//    towerjetreco->Verbosity(verbosity);
//
//    se->registerSubsystem(towerjetreco);
//  }
    
  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(0);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
  se->registerSubsystem( cent );

//    const char *fout = fout_name.c_str();
  // change lower pt and eta cut to make them visible using the example
  //  pythia8 file
  int print_stats_freq = 100;
  CaloJetRhoEst *myJetAnalysis = new CaloJetRhoEst(min_calo_pt, nevnt, print_stats_freq, "AntiKt_Tower_r04", "AntiKt_Truth_r04", fout_name);
  myJetAnalysis->setPtRange(5, 100);
  myJetAnalysis->setEtaRange(-1.1, 1.1);
//    myJetAnalysis->setEtaRange(-2, 2); // this was to confirm that events without a truth lead
//          jet over 10GeV are due to the jet being outside the kinematic acceptance
  myJetAnalysis->add_input(new TowerJetInput(Jet::CEMC_TOWER));
  myJetAnalysis->add_input(new TowerJetInput(Jet::HCALIN_TOWER));
  myJetAnalysis->add_input(new TowerJetInput(Jet::HCALOUT_TOWER));
  se->registerSubsystem(myJetAnalysis);

  // need truth jets
  // need calo  jets
  // need event info
  // need primary vertex
// $ CreateFileList.pl -run 4 -type 11 DST_VERTEX DST_CALO_G4HIT DST_CALO_CLUSTER DST_TRUTH_JET
    
  Fun4AllInputManager *intrue = new Fun4AllDstInputManager("DSTtruth");
  intrue->AddListFile(intrueList,1); // adding the option "1" confirms to use, even if file is large
  se->registerInputManager(intrue);

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
