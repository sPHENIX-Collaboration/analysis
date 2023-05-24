#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllNoSyncDstInputManager.h>
#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>



//#include <g4hitshift/g4hitshift.h>

R__LOAD_LIBRARY(libfun4all.so)
//R__LOAD_LIBRARY(libg4hitshift.so)


void Fun4All_RTrack(const char *g4hitfile = "G4Hits_pythia8_pp_mb-0000000006-00000.root",
const char *filefixedgeo = "updated_geo.root",
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

 

//  g4hitshift *myJetVal = new g4hitshift();
//  se->registerSubsystem(myJetVal);
  
  




  Fun4AllInputManager *intrue = new Fun4AllDstInputManager("DSTcalocluster");
  intrue->Verbosity(2);
  intrue->AddFile(g4hitfile);
  se->registerInputManager(intrue);

  Fun4AllInputManager *intrue2 = new Fun4AllRunNodeInputManager("DSTtruth");
  intrue2->Verbosity(2);
  intrue2->AddFile(filefixedgeo);
  se->registerInputManager(intrue2);



  Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", "crash.root");
  se->registerOutputManager(out);

  se->run(1);
  se->End();

  gSystem->Exit(0);
  return 0;

}
