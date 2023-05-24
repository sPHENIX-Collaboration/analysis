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



R__LOAD_LIBRARY(libfun4all.so)


#endif


void Fun4All_GeoExtract(const char *filegeometry = "G4Hits_sHijing_pAu_0_10fm-0000000006-00000.root",
		    const char * oufilename = "tree_test.root"
		    )
{
  gSystem->Load("libg4dst.so");
  
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

 //===============
  // conditions DB flags
  //===============

  rc->set_StringFlag("CDB_GLOBALTAG","MDC2");
  rc->set_uint64Flag("TIMESTAMP",6);

 


  Fun4AllInputManager *intrue = new Fun4AllDstInputManager("DSTgeo");
  intrue->AddFile(filegeometry);
  se->registerInputManager(intrue);


  Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", "updated_geo.root");
  out->AddRunNode("CYLINDERGEOM_CEMC");
  out->AddRunNode("RunHeader");
  out->AddNode("just_pick_an_invalid_name");
  se->registerOutputManager(out);


  se->run(1);
  se->End();

  gSystem->Exit(0);
  return 0;

}
