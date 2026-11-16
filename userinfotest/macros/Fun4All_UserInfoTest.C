#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <frog/FROG.h>
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/PHTFileServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <phool/recoConsts.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>

#include <userinfotest/userinfotest.h>
#include <metadata/MetaDataTreeWriter.h>

#include <Calo_Calib.C>

R__LOAD_LIBRARY(libuserinfotest.so)
R__LOAD_LIBRARY(libmetadata.so)

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffamodules.so)
#endif
  
void Fun4All_UserInfoTest(const std::string filename = "DST_JETCALO_run2pp_ana521_2025p007_v001-00047289-00000.root",
			  const int nevents = 5)
{ 

  std::pair<int, int> run_seg = Fun4AllUtils::GetRunSegment(filename);

  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();


  const int runnumber = run_seg.first;
  const int segment = run_seg.second;
  std::cout << "runnumber-segment   "<<runnumber <<"-"<<segment<<std::endl;
  rc->set_StringFlag("CDB_GLOBALTAG","newcdbtag");
  rc->set_uint64Flag("TIMESTAMP",54912);

  CDBInterface::instance()->Verbosity(1);

  se->Verbosity(1);


  Fun4AllInputManager *In = new Fun4AllDstInputManager("in");
  In->AddFile(filename.c_str());
  se->registerInputManager(In);

  Process_Calo_Calib();
  
  userinfotest *ufo = new userinfotest("userinfotest","test.root");
  se->registerSubsystem(ufo);

  // provenance tree into the same file (shared through PHTFileServer)
  se->registerSubsystem(new MetaDataTreeWriter("MetaDataTreeWriter", "test.root"));

  se->skip(0);
  se->run(nevents);
  se->End();
  // close the PHTFileServer files now: left open, they are cleaned up at program exit after ROOT
  // already tore its files down, and the job crashes (double free) after finishing
  PHTFileServer::close();
  
  cout << "JOB COMPLETE :)" <<endl;
}
