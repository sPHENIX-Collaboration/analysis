#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <calowaveformsim/CaloWaveFormSim.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalowaveformsim.so)
#endif

void Fun4All_CaloWaveFormSim(const char *fname = "G4Hits_sHijing_0_20fm-0000000041-00000.root", const char *fname2 = "DST_CALO_CLUSTER_sHijing_0_20fm-0000000041-00000.root", const char *outfile = "trees/0.root")
{
  gSystem->Load("libg4dst");
  gSystem->Load("libcalowaveformsim");
  Fun4AllServer *se = Fun4AllServer::instance();
  CaloWaveFormSim *ca = new CaloWaveFormSim("CALOWAVEFORMSIM",outfile);
  ca->Detector("CEMC");
  se->registerSubsystem(ca);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
  in->fileopen(fname);

  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("in2");
  in2->fileopen(fname2);

  // in->AddListFile("g4hits.list");

// Fun4All
  se->registerInputManager(in2);
  se->registerInputManager(in);
  se->run();
  se->End();
}
