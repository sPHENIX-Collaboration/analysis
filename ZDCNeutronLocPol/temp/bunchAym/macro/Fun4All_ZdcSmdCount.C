#include <ZdcSmdCount.h>

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
R__LOAD_LIBRARY(libZdcSmdCount.so)


void Fun4All_ZdcSmdCount(int nevents =0,int runnumber = 42796 , float cut_value = 50,int max_roller = 50,int file_num=0)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();

  gSystem->Load("libZdcSmdCount.so.0.0.0");
  ZdcSmdCount *zs = new ZdcSmdCount("SimpleAna",runnumber,cut_value,file_num);
  se->registerSubsystem(zs);
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
  int first_file = max_roller*file_num;
  int last_file = max_roller*(file_num+1);
  for(int i=first_file;i<last_file;i++)
  {
	char filename_c[200];
	sprintf(filename_c, "/sphenix/lustre01/sphnxpro/physics/slurp/calophysics/run_00042700_00042800/DST_TRIGGERED_EVENT_run2pp_new_2024p001-%08d-%04d.root", runnumber, i);
	std::string filename(filename_c);
	in->AddFile(filename);
  }
  in->Print();
  se->registerInputManager(in);

  se->run(nevents);
  se->End();
  //se->PrintTimer();
  gSystem->Exit(0);

  return 0;
}
