#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <mbdinfo/MBDinfo.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4dst.so)
R__LOAD_LIBRARY(libMBDinfo.so)


#endif

//dst_mbd_epd.list created with: CreateFileList.pl -type 4 -run 10 DST_MBD_EPD

void Fun4Allsim(int nevents = 1e4, const string &inputmbdepdList = "dst_mbd_epd.list")
{


  gSystem->Load("libg4dst.so");
  Fun4AllServer *se = Fun4AllServer::instance();
    
  gSystem->Load("libMBDinfo.so.0.0.0");
  MBDinfo *MBDreco = new MBDinfo();
  se->registerSubsystem(MBDreco);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTMBDEPD");
  in->AddListFile(inputmbdepdList,1);
  se->registerInputManager(in);

  se->run(nevents);
  se->End();

  gSystem->Exit(0);
  return 0;

}
