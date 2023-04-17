#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <g4jets/FastJetAlgo.h>
#include <g4jets/JetReco.h>
#include <g4jets/TowerJetInput.h>
#include <g4jets/TruthJetInput.h>

#include <g4centrality/PHG4CentralityReco.h>

#include <jetbackground/CopyAndSubtractJets.h>
#include <jetbackground/DetermineTowerBackground.h>
#include <jetbackground/FastJetAlgoSub.h>
#include <jetbackground/RetowerCEMC.h>
#include <jetbackground/SubtractTowers.h>
#include <jetbackground/SubtractTowersCS.h>


#include <G4_Bbc.C>
#include <G4_CaloTrigger.C>
#include <G4_Centrality.C>
#include <G4_DSTReader.C>
#include <G4_Global.C>
#include <G4_Input.C>
#include <G4_KFParticle.C>
#include <G4_ParticleFlow.C>
#include <G4_Production.C>
#include <G4_TopoClusterReco.C>
#include <G4_Tracking.C>
#include <G4_User.C>
#include <QA.C>
#include "HIJetReco.C"

// #include <G4_CEmc_Spacal.C>
#include <jetrtrack/jetrtrack.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libjetrtrack.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4dst.so)

#endif


void Fun4All_RTrack(const char *filetruthjet = "DST_TRACKS_pythia8_Jet30-0000000062-00000.root",
                     const char *filecalo = "DST_CALO_CLUSTER_pythia8_Jet30-0000000062-00000.root",  
		    const char *filetruthjets = "DST_TRUTH_JET_pythia8_Jet30-0000000062-00000.root",
		    const char *filetruth = "DST_TRUTH_pythia8_Jet30-0000000062-00000.root",
		    const char * oufilename = "tree_test.root"
		    )
{

  
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();


 

  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(0);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
  se->registerSubsystem( cent );
  
  HIJetReco(); // Performs the officially reccomended jet reconstruction


  jetrtrack *myJetVal = new jetrtrack("analysis");
  myJetVal->SetOutputFileName(oufilename);
  se->registerSubsystem(myJetVal);
  
  

  Fun4AllInputManager *intrue = new Fun4AllDstInputManager("DSTcalocluster");
  intrue->AddFile(filetruthjet);
  se->registerInputManager(intrue);

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalo");
  in2->AddFile(filecalo);
  se->registerInputManager(in2);

  Fun4AllInputManager *in3 = new Fun4AllDstInputManager("DSTtruthjets");
  in3->AddFile(filetruthjets);
  se->registerInputManager(in3);

  Fun4AllInputManager *in4 = new Fun4AllDstInputManager("DSTtrackseeds");
  in4->AddFile(filetruth);
  se->registerInputManager(in4);


  se->run();
  se->End();

  gSystem->Exit(0);
  return 0;

}
