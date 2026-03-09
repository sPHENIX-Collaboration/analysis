// these include guards are not really needed, but if we ever include this
// file somewhere they would be missed and we will have to refurbish all macros
#ifndef MACRO_FUN4ALLRUNEMCRECO_C
#define MACRO_FUN4ALLRUNEMCRECO_C

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <../src/cemcReco.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcemcReco.so)

void Fun4All_RunEMCReco(
    const int nEvents = 1,
    const string caloRecoIn = "dst_calo_cluster_run40_JS30GeV_noPileup.list",
    const string caloTruthIn = "dst_truth_run40_JS30GeV_noPileup.list",
    const string tracksIn = "",
    const string &outputFile = "cemcReco_Out.root"
			)
{
  // this convenience library knows all our i/o objects so you don't
  // have to figure out what is in each dst type
  gSystem->Load("libg4dst.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);  // set it to 1 if you want event printouts

  Fun4AllInputManager *inCluster = new Fun4AllDstInputManager("DSTClusters");
  std::cout << "Adding file list " << caloRecoIn << std::endl;
  inCluster -> AddListFile(caloRecoIn,1);
  se->registerInputManager(inCluster);  
  
  if(strcmp(tracksIn.c_str(),""))
    {
      Fun4AllInputManager *inTracks = new Fun4AllDstInputManager("DSTTracks");
      std::cout << "Adding file list " << tracksIn  << std::endl;
      inTracks -> AddListFile(tracksIn,1);
      se->registerInputManager(inTracks);
    }
  
  Fun4AllInputManager *truthCalo = new Fun4AllDstInputManager("DSTCaloTruth");
  std::cout << "Adding file list " << caloTruthIn  << std::endl;
  truthCalo -> AddListFile(caloTruthIn,1);
  se -> registerInputManager(truthCalo);

  cemcReco *eval = new cemcReco("cemcRecoAna", outputFile);
  se->registerSubsystem(eval);

  se->run(nEvents);
  se->End();
  
  delete se;
  cout << "Analysis Completed" << endl;
  
  gSystem->Exit(0);
}

#endif  //MACRO_FUN4ALLRUNEMCRECO_C
