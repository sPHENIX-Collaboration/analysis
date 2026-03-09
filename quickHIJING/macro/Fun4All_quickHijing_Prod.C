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


#include <quickhijing/quickHIJING.h>

#include <g4centrality/PHG4CentralityReco.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libg4centrality.so)
R__LOAD_LIBRARY(libquickHIJING.so)

#endif


void Fun4All_quickHijing_Prod(int nEvents = 1,
                     const char *filelist1 = "dst_truth.list",
		     const string sysName = "qhTest")
{

  gSystem->Load("libg4dst");
  
  
  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();
  //rc->set_IntFlag("RUNNUMBER",62);

  PHG4CentralityReco *cent = new PHG4CentralityReco();
  cent -> Verbosity(0);
  cent -> GetCalibrationParameters().ReadFromFile("centrality","xml",0,0,string(getenv("CALIBRATIONROOT"))+string("/Centrality/"));
  se -> registerSubsystem(cent);

  quickHIJING *qh = new quickHIJING(sysName);
  se -> registerSubsystem(qh);
  
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  in->AddListFile(filelist1,1);

  se->registerInputManager(in);
  se->run(nEvents);
  se->End();
  std::cout << "All done!" << endl;
  gSystem -> Exit(0);
  

 
}
