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

#include <g4centrality/PHG4CentralityReco.h>

#include <HIJetReco.C>
#include "jetvalidation/EMJetVal.h"
//#include </sphenix/user/jamesj3j3/analysis/JS-Jet/JetValidation/src/EMJetVal.h>
//#include </sphenix/user/jamesj3j3/analysis/JS-Jet/JetValidation/src/JetValidation.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libJetValidation.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4dst.so)


#endif


void Fun4All_EMJetVal(const char *filelisttruth = "dst_truth_jet.list",
                    const char *filelistcalo = "dst_calo_cluster.list",
		    const char *filelistglobal = "dst_global.list",
		    const char *outname = "output.root")
{

  
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  
  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(0);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
  se->registerSubsystem( cent );
  
  HIJetReco();
 

  // JetValidation *myJetVal = new JetValidation("AntiKt_Tower_r04_Sub1", "AntiKt_Truth_r04", outname);
  
  EMJetVal *myEMJetVal = new EMJetVal("AntiKt_Tower_r04_Sub1","AntiKt_Truth_r04", outname);

  myEMJetVal->setPtRange(5, 100);
  myEMJetVal->setEtaRange(-1.1, 1.1);
  myEMJetVal->doUnsub(1);
  myEMJetVal->doTruth(0);
  myEMJetVal->doSeeds(0);
  //  se->registerSubsystem(myJetVal);
  se->registerSubsystem(myEMJetVal);
  

  Fun4AllInputManager *intrue = new Fun4AllDstInputManager("DSTtruth");
  //intrue->AddListFile(filelisttruth,1);
  // turn off for running data
  // se->registerInputManager(intrue);

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalo");
  in2->AddListFile(filelistcalo,1);
  // se->registerInputManager(in2);
  se->registerInputManager(in2);

  Fun4AllInputManager *in3 = new Fun4AllDstInputManager("DSTglobal");
  // in3->AddListFile(filelistglobal,1);
  // turn off for running data
  // se->registerInputManager(in3);

  se->run(10000);
  se->End();

  gSystem->Exit(0);
  return 0;

}
