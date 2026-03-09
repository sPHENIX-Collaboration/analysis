#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <Detinfo.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libDetinfo.so)


#endif

void Fun4All_calo(int nevents = 1e0)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();

  gSystem->Load("libDetinfo.so.0.0.0");
  Detinfo *detana = new Detinfo();
  detana->set_filename("calotowerinfo.root");
  se->registerSubsystem(detana);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  
  in->AddFile("/sphenix/lustre01/sphnxpro/commissioning/slurp/caloy2test/run_00042200_00042300/DST_CALO_run2pp_new_2024p001-00042297-0070.root");

  se->registerInputManager(in);

  se->run(nevents);
  se->End();
  se->PrintTimer();
  gSystem->Exit(0);
   
  return 0;
}
