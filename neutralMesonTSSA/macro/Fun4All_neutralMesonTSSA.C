#pragma once

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <GlobalVariables.C>
#include <G4_Global.C>
#include <G4_Mbd.C>

#include <neutralmesontssa/neutralMesonTSSA.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libneutralMesonTSSA.so)

void Fun4All_neutralMesonTSSA(
                     int nEvents = 1,
                     const char *filelist1 = "dst_calo_cluster.list",
                     const char *filelist2 = "dst_truth.list",
		     const string outname = "neutralMesonTSSA_hists.root",
		     bool isMC = true)
{
  // this convenience library knows all our i/o objects so you don't
  // have to figure out what is in each dst type
  gSystem->Load("libg4dst.so");
  /* gSystem->Load("libneutralMesonTSSA.so"); */

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);  // set it to 1 if you want event printouts

  Fun4AllInputManager *inCluster = new Fun4AllDstInputManager("DSTCaloCluster");
  std::cout << "Adding file list " << filelist1 << std::endl;
  inCluster->AddListFile(filelist1,1);
  se->registerInputManager(inCluster);

  Fun4AllInputManager *inTruth = new Fun4AllDstInputManager("DSTTruth");
  if (isMC) {
      std::cout << "Adding file list " << filelist2 << std::endl;
      inTruth -> AddListFile(filelist2,1);
      se -> registerInputManager(inTruth);
  }

  neutralMesonTSSA *eval = new neutralMesonTSSA("neutralMesonTSSA", isMC);
  eval->set_min_clusterE(0.3);
  eval->set_max_clusterChi2(4.0);
  se -> registerSubsystem(eval);
  
  se->run(nEvents);
  se->End();
  
  delete se;
  cout << "Analysis Completed" << endl;
  
  gSystem->Exit(0);
}

