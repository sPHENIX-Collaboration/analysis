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


#include "upc/hijbkg_upc.h"

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libhbcup.so)

#endif


void Fun4All_hijbkg(int nEvents = 0, const char *dst_fname = "DST_TRUTH_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000006-00000.root", const string sysName = "HIJBKG")
{

  gSystem->Load("libg4dst");
  
  
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();
  //rc->set_IntFlag("RUNNUMBER",62);

  hijbkg_upc *hijbkg = new hijbkg_upc(sysName);
  se -> registerSubsystem(hijbkg);
  
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_MDC2");
  //in->AddListFile(filelist1,1);
  in->AddFile( dst_fname );

  se->registerInputManager( in );
  se->run( nEvents );
  se->End();
  std::cout << "All done!" << endl;
  gSystem->Exit( 0 );
  
}
