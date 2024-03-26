#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <jetnconstituents/JetNconstituents.h>


#include <HIJetReco.C>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4jets.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libJetNconstituents.so)

#endif

void Fun4All_jetQA(const int nEvents = 10,
		   const char *filelistcalo = "dst_calo_cluster.list",
		   const char *filelistglobal = "dst_global.list",
		   const char *outname = "test.root")
{

  //make the fun4all server
  Fun4AllServer *se = Fun4AllServer::instance();

  //set the verbosity (increase to print more output for debugging)
  int verbosity = 0;
  se->Verbosity(verbosity);
  Enable::VERBOSITY = verbosity;

  //run the jet reconstruction
  HIJetReco();

  //run your module
  //replace TestModule with your own module
  JetNconstituents *myJetNconst = new JetNconstituents(outname);
  myJetNconst->setPtRange(1.0, 1000);
  myJetNconst->setEtaRange(-1.1, 1.1);
  myJetNconst->setRecoJetNodeName("AntiKt_Tower_r04_Sub1");

  se->registerSubsystem(myJetNconst);

  //add inputs
  //add any additional needed inputs here
  Fun4AllInputManager *incalo = new Fun4AllDstInputManager("DSTcalo");
  incalo->AddListFile(filelistcalo,1);
  se->registerInputManager(incalo);

  Fun4AllInputManager *inglobal = new Fun4AllDstInputManager("DSTglobal");
  inglobal->AddListFile(filelistglobal,1);
  se->registerInputManager(inglobal);

  //run
  se->run(nEvents);
  se->End();

  gSystem->Exit(0);
  return 0;
}
