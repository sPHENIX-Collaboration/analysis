#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <centrality/CentralityReco.h>
#include <centralityvalid/CentralityValidation.h>
#include <calotrigger/MinimumBiasClassifier.h>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <vector>

#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libcentrality.so)
R__LOAD_LIBRARY(libcentralityvalid.so)
R__LOAD_LIBRARY(libcalotrigger.so)
#endif

void Fun4All_CentralityValidation(const int runnumber)
{
  gSystem->Load("libg4dst");
  gSystem->Load("libcentrality");
  gSystem->Load("libcentralityvalid");
  gSystem->Load("libcalotrigger");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  recoConsts *rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG","ProdA_2023");
  // // 64 bit timestamp                                                                                                                                
  rc->set_uint64Flag("TIMESTAMP",runnumber);


  Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
  in->fileopen(Form("/sphenix/lustre01/sphnxpro/commissioning/DST_ana395_2023p007/DST_CALO_run1auau_ana395_2023p007-000%d-0000.root", runnumber));
  se->registerInputManager(in);

  // Fun4All
  CentralityReco *cr = new CentralityReco();
  cr->Verbosity(1);
  se->registerSubsystem(cr);
  
  MinimumBiasClassifier *mb = new MinimumBiasClassifier();
  mb->Verbosity(0);
  se->registerSubsystem(mb);

  CentralityValidation *centralityvalidation = new CentralityValidation("CentralityValidation","cent_valid.root");
  se->registerSubsystem(centralityvalidation);

  se->run(10);
  se->End();
}
