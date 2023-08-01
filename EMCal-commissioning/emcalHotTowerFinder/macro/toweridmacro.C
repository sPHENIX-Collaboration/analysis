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

#include <towerid/towerid.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libtowerid.so)
//R__LOAD_LIBRARY(libmakeMBDTrees.so)
#endif
void toweridmacro(const int nEvents = 500000, const char *listFile = "/sphenix/lustre01/sphnxpro/commissioning/DST/run_20107/dsts_fast/00020107-0150.root", const char *inName = "commissioning_loeta_200.root", float adccut = 200, float sigmas = 4, int nbins = 50000)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();

  towerid *calo = new towerid(inName,adccut,sigmas,nbins);
  se->registerSubsystem(calo);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  in->AddFile(listFile);
  se->registerInputManager(in);

  se->run(nEvents);
  se->End();
  se->PrintTimer();
  std::cout << "All done!" << std::endl;

  gSystem->Exit(0);
}

