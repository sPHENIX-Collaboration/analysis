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


#include <jetnconstituents/JetNconstituents.h>

// #include <jetvalidation/JetValidation.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libJetNconstituents.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libg4dst.so)


#endif

void Fun4All_JetNconst(const char *filelistcalo = "/sphenix/u/tmengel/dst_calo_cluster.list",  
                     const char *outname = "outputest.root")
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

  JetNconstituents *myJetNconst = new JetNconstituents("AntiKt_Tower_r04_Sub1", outname);

  myJetNconst->setPtRange(5, 100);
  myJetNconst->setEtaRange(-1.1, 1.1);
  se->registerSubsystem(myJetNconst);

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("DSTcalo");
  in2->AddListFile(filelistcalo,1);
  se->registerInputManager(in2);

  
  se->run(-1);
  se->End();

  gSystem->Exit(0);
  return 0;

}
