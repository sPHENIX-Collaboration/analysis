// these include guards are not really needed, but if we ever include this
// file somewhere they would be missed and we will have to refurbish all macros
#ifndef MACRO_FUN4ALLISOLATEDTRACKS_C
#define MACRO_FUN4ALLISOLATEDTRACKS_C

#include <G4Setup_sPHENIX.C>

#include <G4_Tracking.C>

#include <trackreco/PHActsTrackProjection.h>
#include <trackreco/PHActsTrackProjection.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <isolatedtrackanalysis/IsolatedTrackAnalysis.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libIsolatedTrackAnalysis.so)

void Fun4All_IsolatedTrackAnalysis(
      const int nEvents = 10,
      const string &g4HitsFile = "g4hits.list",
      const string &trkrClusterFile = "dst_trkr_cluster.list",
      const string &trackFile = "dst_tracks.list",
      const string &clusterFile = "dst_calo_cluster.list",
      const string &truthFile = "dst_truth.list",
      const string &outputFile = "isotrack_output.root"
      )
{
  // this convenience library knows all our i/o objects so you don't
  // have to figure out what is in each dst type
  gSystem->Load("libg4dst.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);  // set it to 1 if you want event printouts

  //Fun4AllInputManager *inTrack = new Fun4AllDstInputManager("DSTTracks");
  //std::cout << "Adding file list " << trackFile << std::endl;
  //inTrack->AddListFile(trackFile,1);
  //se->registerInputManager(inTrack);

  Fun4AllInputManager *inCluster = new Fun4AllDstInputManager("DSTClusters");
  std::cout << "Adding file list " << clusterFile << std::endl;
  inCluster->AddListFile(clusterFile,1);
  se->registerInputManager(inCluster);  

  Fun4AllInputManager *truthTracks = new Fun4AllDstInputManager("DSTTruth");
  std::cout << "Adding file list " << truthFile  << std::endl;
  truthTracks->AddListFile(truthFile,1);
  se->registerInputManager(truthTracks);

  IsolatedTrackAnalysis *analysisModule = new IsolatedTrackAnalysis("IsolatedTrackAnalysis", outputFile);
  analysisModule->analyzeTracks(false);
  analysisModule->analyzeClusters(true);
  analysisModule->analyzeTowers(false);
  analysisModule->analyzeSimTowers(false);
  analysisModule->analyzeHepMCTruth(false);
  analysisModule->analyzeG4Truth(false);
  analysisModule->setMinClusterEnergy(0.1);
  analysisModule->setMinTowerEnergy(0.04);
  analysisModule->setMinSimTowerEnergy(0.04);
 
  se->registerSubsystem(analysisModule);
  
  se->run(nEvents);
  se->End();
  
  delete se;
  cout << "Analysis Completed" << endl;
  
  gSystem->Exit(0);
}

#endif  //MACRO_FUN4ALLG4SLOPECAL_C
