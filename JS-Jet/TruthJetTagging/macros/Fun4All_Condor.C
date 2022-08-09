#ifndef MACRO_FUN4ALLCONDOR_C
#define MACRO_FUN4ALLCONDOR_C
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
#include <jetbackground/CopyAndSubtractJets.h>
#include <jetbackground/DetermineTowerBackground.h>
#include <jetbackground/FastJetAlgoSub.h>
#include <jetbackground/RetowerCEMC.h>
#include <jetbackground/SubtractTowers.h>
#include <jetbackground/SubtractTowersCS.h>

#include <G4_Jets.C>
#include <frog/FROG.h>
#include "truthjettagging/TruthJetTagging.h"

R__LOAD_LIBRARY(libfun4all.so)

R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libTruthJetTagging.so)


namespace G4HIJETS
{
  int do_flow = 1;
  bool do_flow_subtraction = true;
  bool do_CS = false;
} 


void Fun4All_Condor(string inputFile1 = "DST_TRUTH_pythia8_Bottom_3MHz-0000000003-00000.root",  int outputfilenumber = 0)
  {
const int nEvents = 10;

  gSystem->Load("libFROG");

  FROG *fr = new FROG();
  cout << fr->location(inputFile1.c_str()) << endl;


  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);
  int verbosity = 0;

  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
  hitsin->fileopen(inputFile1);
  se->registerInputManager(hitsin);

  //-------------------------------------------------------------------------------------------
  // Reconstruct truth jets if they are not already in the node tree
  //-------------------------------------------------------------------------------------------

  JetReco *truthjetreco = new JetReco("TRUTHJETRECO");
  TruthJetInput *tji = new TruthJetInput(Jet::PARTICLE);
  tji->add_embedding_flag(1);  // (1) for pythia simulations, (2) for pythia embedding into hijing
  truthjetreco->add_input(tji);
  truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.4), "AntiKt_Truth_r04");
  truthjetreco->set_algo_node("ANTIKT");
  truthjetreco->set_input_node("TRUTH");
  truthjetreco->Verbosity(0);
  se->registerSubsystem(truthjetreco);


 TruthJetTagging *myreco = new TruthJetTagging();
 myreco->add_algo("AntiKt_Truth_r04"); // add each truth jet node to look at here
 myreco->add_radius(0.4); // If doing HF tagging must specify one radius for each truth jet node, in order!
 myreco->do_hf_tagging(true); // enable HF tagging
 myreco->set_embedding_id(1); // (1) for pythia simulations, (2) for pythia embedding into hijing

 myreco->do_photon_tagging(true); // enable photon jet tagging (returns fraction of jet pT carried by the highest pT photon
 se->registerSubsystem(myreco);
  

string FullOutFile = "testoutput.root";
Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", FullOutFile);
se->registerOutputManager(out);
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

