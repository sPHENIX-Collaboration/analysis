#ifndef MACRO_FUN4ALLANALYSIS_C
#define MACRO_FUN4ALLANALYSIS_C
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDummyInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include <g4jets/ClusterJetInput.h>
#include <g4jets/FastJetAlgo.h>
#include <g4jets/JetReco.h>
#include <g4jets/TowerJetInput.h>
#include <g4jets/TrackJetInput.h>
#include <g4jets/TruthJetInput.h>
#include <G4_HIJetReco.C>
#include <G4_Jets.C>
#include <analysis/analysis.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libanalysis.so)

void Fun4All_Analysis(string inputFile1 = "DST_TRUTH_JET_pythia8_Jet04_3MHz-0000000004-00000.root", string inputFile2 = "DST_CALO_CLUSTER_pythia8_Jet04_3MHz-0000000004-00000.root",string outputfilename = "test.root", const int nEvents = 100)
{
  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  //-----------------------------------------------------------------------
  //Read in a file containing the truth jet information
  //-----------------------------------------------------------------------
  Fun4AllInputManager *t_in = new Fun4AllDstInputManager("DSTin");
  t_in->fileopen(inputFile1);
  se->registerInputManager(t_in);

  //---------------------------------------------------------------------------
  //Read in a file containing the calorimeter information
  //---------------------------------------------------------------------------

 Fun4AllInputManager*  in = new Fun4AllDstInputManager("DSTCALOCLUSTERS");
 in->fileopen(inputFile2);
 se->registerInputManager(in);
 //----------------------------------------------------------------------------------------------
 //Perform the jet reconstruction: by default producing 0.2-0.8 jets
 //----------------------------------------------------------------------------------------------

  JetReco *towerjetreco = new JetReco("TOWERJETRECO");
  towerjetreco->add_input(new TowerJetInput(Jet::CEMC_TOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWER));
  towerjetreco->add_input(new TowerJetInput(Jet::HCALOUT_TOWER));
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.2), "AntiKt_Tower_r02");
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.3), "AntiKt_Tower_r03");
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.4), "AntiKt_Tower_r04");
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.5), "AntiKt_Tower_r05");
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.6), "AntiKt_Tower_r06");
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.7), "AntiKt_Tower_r07");
  towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.8), "AntiKt_Tower_r08");
  towerjetreco->set_algo_node("ANTIKT");
  towerjetreco->set_input_node("TOWER");
  se->registerSubsystem(towerjetreco);


  //-----------------------------------------------
  // register up the analysis module
  //-----------------------------------------------

  analysis *myreco = new analysis();
  myreco->SetOutputFileName(outputfilename);
  se->registerSubsystem(myreco);
  
  if (nEvents <= 0)
  {
    return;
  }
  cout << endl << "Running over " << nEvents << " Events" << endl;
  se->run(nEvents);
  cout << endl << "Calling End in Fun4All_Analysis.C" << endl;
  se->End();
  cout << endl << "All done, calling delete Fun4AllServer" << endl;
  delete se;
  cout << endl << "gSystem->Exit(0)" << endl;
  gSystem->Exit(0);
}
#endif // MACRO_FUN4ALLANALYSIS_C

