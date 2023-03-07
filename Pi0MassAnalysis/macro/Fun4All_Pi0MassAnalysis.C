// these include guards are not really needed, but if we ever include this
// file somewhere they would be missed and we will have to refurbish all macros
#ifndef MACRO_FUN4ALLPI0MASSANALYSIS_C
#define MACRO_FUN4ALLPI0MASSANALYSIS_C


#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <pi0massanalysis/Pi0MassAnalysis.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libPi0MassAnalysis.so)

void Fun4All_MDCTreeMaker(
      const int nEvents = 1000,
      const string &clusterFile = "dst_calo_cluster.list",
      const string &truthFile = "dst_truth.list",
      const string &outputFile = "AuAu_minbias_output.root"
      )
{
  // this convenience library knows all our i/o objects so you don't
  // have to figure out what is in each dst type
  gSystem->Load("libg4dst.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);  // set it to 1 if you want event printouts

  Fun4AllInputManager *inCluster = new Fun4AllDstInputManager("DSTClusters");
  std::cout << "Adding file list " << clusterFile << std::endl;
  inCluster->AddListFile(clusterFile,1);
  se->registerInputManager(inCluster);  

  //Fun4AllInputManager *truthTracks = new Fun4AllDstInputManager("DSTTruth");
  //std::cout << "Adding file list " << truthFile  << std::endl;
  //truthTracks->AddListFile(truthFile,1);
  //se->registerInputManager(truthTracks);

  Pi0MassAnalysis *analysisModule = new Pi0MassAnalysis(outputFile);
  analysisModule->SetMinClusterEnergy(1.0);
  se->registerSubsystem(analysisModule);
  
  se->run(nEvents);
  se->End();
  
  delete se;
  cout << "Analysis Completed" << endl;
  
  gSystem->Exit(0);
}

#endif 
