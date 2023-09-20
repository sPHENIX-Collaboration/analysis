#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <phool/recoConsts.h>
#include <fun4all/Fun4AllNoSyncDstInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

// #include <calotowerbuilder/CaloTowerBuilder.h>
#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloWaveformProcessing.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <caloana/CaloAna.h>
#include <fun4all/Fun4AllDstOutputManager.h>
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libcaloana.so)
R__LOAD_LIBRARY(libffamodules.so)
#endif

void Fun4All_Calo(const char* infile = "/gpfs/mnt/gpfs02/sphenix/user/trinn/comissioning_data_production/test_file.root",const char *outfile = "tim_calo_23699.root")
{


 

  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

 //===============
  // conditions DB flags
  //===============

  // global tag
  rc->set_StringFlag("CDB_GLOBALTAG","MDC2");
  // // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP",6);


Fun4AllInputManager *intrue = new Fun4AllDstInputManager("DST_TOWERS");
  intrue->AddFile(infile);
se->registerInputManager(intrue);


 CaloAna *ca = new CaloAna("calomodulename",outfile);
  se->registerSubsystem(ca);






  se->run();
  se->End();

}
