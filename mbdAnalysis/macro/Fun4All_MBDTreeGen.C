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

#include <mbdanalysis/mbdAnalysis.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libmbdAnalysis.so)
R__LOAD_LIBRARY(libmbd_io.so)
R__LOAD_LIBRARY(libmbd.so)
#endif

void Fun4All_MBDTreeGen(const int nEvents = 0, const char *listFile = "mbdFileList.list", const char *inName = "commissioning.root")
{
  gSystem->Load("libg4dst");
  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();

  mbdAnalysis *mbdAnalysis_pls = new mbdAnalysis(inName);
  se->registerSubsystem(mbdAnalysis_pls);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  //in->AddListFile(listFile);
  in->AddFile(listFile);
  //in -> AddFile("/sphenix/lustre01/sphnxpro/zfs/rucio/group/sphenix/bd/1e/DST_CALOR_ana383_2023p002-00021598-0000.root");
  se->registerInputManager(in);

  se->run(nEvents);
  se->End();
  se->PrintTimer();
  std::cout << "All done!" << std::endl;

  gSystem->Exit(0);
}
