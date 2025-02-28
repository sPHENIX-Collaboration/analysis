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

#include <jetunderlyingevent/JetUnderlyingEvent.h>


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(JetUnderlyingEvent.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4dst.so)


#endif

void Fun4All_JetVal(const char *filelisttruth = "dst_03_27/dst_truth_jet.list",
		    const char *filelistcalo = "dst_03_27/dst_calo_cluster.list",
		    const char *filelistglobal = "dst_03_27/dst_global.list",
		    const char *filelistg4hit = "dst_03_27/dst_bbc_g4hit.list",  
		    const char *outname = "outputest.root")
{

  
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2023");  

  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent->Verbosity(verbosity);
  cent->GetCalibrationParameters().ReadFromFile("centrality", "xml", 0, 0, string(getenv("CALIBRATIONROOT")) + string("/Centrality/"));
  se->registerSubsystem( cent );
<<<<<<< HEAD:JS-Jet/JetValidation/macro/Fun4All_JetVal.C
  
  Enable::VERBOSITY = verbosity;
  HIJetReco();

  JetValidation *myJetVal = new JetValidation("AntiKt_Tower_r04_Sub1", "AntiKt_Truth_r04", outname);
=======
>>>>>>> 07b79f5351fbab0f10c9a719c1cb49f741419ec1:JS-Jet/JetUnderlyingEvent/macro/Fun4All_JetVal.C

  HIJetReco();
 

  JetUnderlyingEvent *myJetVal = new JetUnderlyingEvent("name",outname);
  se->registerSubsystem(myJetVal); 
  
  Fun4AllInputManager *intrue = new Fun4AllDstInputManager("DSTtruth");
  intrue->AddListFile(filelisttruth,1);
  se->registerInputManager(intrue);
  
  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalo");
  in2->AddListFile(filelistcalo,1);
  se->registerInputManager(in2);
  
  Fun4AllInputManager *in3 = new Fun4AllDstInputManager("DSTglobal");
  in3->AddListFile(filelistglobal,1);
  se->registerInputManager(in3);
  
  se->run(-1);

  se->End();

  gSystem->Exit(0);
  return 0;

}
