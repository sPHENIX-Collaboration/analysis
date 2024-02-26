// these include guards are not really needed, but if we ever include this

#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllSyncManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllNoSyncDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <centrality/CentralityReco.h>

#include <caloreco/CaloTowerStatus.h>

#include <jetbackground/RetowerCEMC.h>
#include <jetbackground/FastJetAlgoSub.h>

#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>
#include <jetbase/FastJetAlgo.h>
#include <g4jets/TruthJetInput.h>

#include <caloembedding/caloTowerEmbed.h>

#include <jetvalidation/EventSelection.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libJetBkgdSub.so)
R__LOAD_LIBRARY(libCaloEmbedding.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libfun4all.so)
//R__LOAD_LIBRARY(libEventSelection.so)

void Fun4All_EventSelection(
				       const int nEvents = 1,
				       const string &clusterFile = "dst_calo_cluster.list",
				       const string &truthFile = "dst_truth.list",
				       const string &outputFile = "eventselectest.root"
				       )
{
  // this convenience library knows all our i/o objects so you don't
  // have to figure out what is in each dst type
  gSystem->Load("libg4dst.so");
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  
  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(0);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
  se->registerSubsystem( cent );
  
  HIJetReco();
 
  
  EventSelection *myEventSelection = new EventSelection("AntiKt_Tower_r04_Sub1","AntiKt_Truth_r04", outname);

  myEventSelection->setPtRange(5, 100);
  myEventSelection->setEtaRange(-1.1, 1.1);
  myEventSelection->doUnsub(1);
  myEventSelection->doTruth(0);
  myEventSelection->doSeeds(0);
  //  se->registerSubsystem(myJetVal);
  se->registerSubsystem(myEventSelection);
  Fun4AllSyncManager *sync = se->getSyncManager();
  sync->MixRunsOk(true);

  Fun4AllInputManager *inData = new Fun4AllDstInputManager("DSTData","DST");
  inData->AddListFile(filelistdata);
  se->registerInputManager(inData);
  
  se->run(nEvents);
  se->End();
  
  delete se;
  cout << "Analysis Completed" << endl;
  
  gSystem->Exit(0);
}

#endif  //MACRO_FUN4ALLEVENTSELECTION_C
