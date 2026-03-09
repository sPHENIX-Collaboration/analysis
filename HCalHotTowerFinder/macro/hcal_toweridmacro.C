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

#include <hcal_towerid/hcal_towerid.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libtowerid.so)
//R__LOAD_LIBRARY(libmakeMBDTrees.so)
#endif
void hcal_toweridmacro(const int nEvents = 5000, const char *listFile = "/sphenix/lustre01/sphnxpro/commissioning/DST/run_22950/dsts_fast/DST-00022950-0018.root", const char *inName = "commissioning.root",const std::string cdbname_i = "cdbname_i.root",const std::string cdbname_o = "cdbname_o.root",  float adccut_i = 150, float adccut_o = 150, float sigmas_lo = 2, float sigmas_hi = 4, float inner_f = 0.05, float outer_f = 0.05)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();

  hcal_towerid *calo = new hcal_towerid(inName,cdbname_i,cdbname_o,adccut_i,adccut_o,sigmas_lo,sigmas_hi,inner_f,outer_f);
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

