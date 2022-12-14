// these include guards are not really needed, but if we ever include this
// file somewhere they would be missed and we will have to refurbish all macros
#ifndef MACRO_FUN4ALLRUNPI0EFFICIENCY_C
#define MACRO_FUN4ALLRUNPI0EFFICIENCY_C

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <../src/pi0ClusterAna.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libpi0ClusterAna.so)

void Fun4All_RunPi0ClusterAna(
      const int nEvents = 1,
      const string &clusterFile = "dst_calo_cluster.list",
      const string &truthFile = "dst_truth.list",
      const string &outputFile = "cemcClusterAna_Out.root"
			)
{
  // this convenience library knows all our i/o objects so you don't
  // have to figure out what is in each dst type
  gSystem->Load("libg4dst.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);  // set it to 1 if you want event printouts

  Fun4AllInputManager *inCluster = new Fun4AllDstInputManager("DSTClusters");
  std::cout << "Adding file list " << clusterFile << std::endl;
  inCluster -> AddListFile(clusterFile,1);
  se->registerInputManager(inCluster);  

  Fun4AllInputManager *truthCalo = new Fun4AllDstInputManager("DSTCaloTruth");
  std::cout << "Adding file list " << truthFile  << std::endl;
  truthCalo -> AddListFile(truthFile,1);
  se -> registerInputManager(truthCalo);

  pi0ClusterAna *eval = new pi0ClusterAna("dummy", outputFile);
  se->registerSubsystem(eval);
  
  se->run(nEvents);
  se->End();
  
  delete se;
  cout << "Analysis Completed" << endl;
  
  gSystem->Exit(0);
}

#endif  //MACRO_FUN4ALLG4SLOPECAL_C
