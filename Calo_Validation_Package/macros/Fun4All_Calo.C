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

void Fun4All_Calo(int nevents = 10,const std::string &fname = "/sphenix/lustre01/sphnxpro/commissioning/DSTv3/DST_CALOR-00021598-0001.root")
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


  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_TOWERS");
  //in->AddFile("/sphenix/tg/tg01/jets/ahodges/run23_production_zvertex/21813/DST-00021813-0010.root");      
  in->AddFile(fname);
  se->registerInputManager(in);

  std::string filename = fname.substr(fname.find_last_of("/\\") + 1);
  std::string OutFile = "CALOHIST_" + filename;

  CaloAna *ca = new CaloAna("calomodulename",OutFile);
  ca->set_timing_cut_width(2);  //integers for timing width, > 1 : wider cut around max peak time
  ca->apply_vertex_cut(true);  
  ca->set_vertex_cut(20.);
  se->registerSubsystem(ca);

  se->run(nevents); //update number of events as needed
  se->End();

}
