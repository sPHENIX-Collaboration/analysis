#include "Fun4All_InttTriggerTiming.hh"

int Fun4All_InttTriggerTiming( int run_num = 50377,
				 int trigger_bit = -1  // -1 means no requirement
				 )
{
  
  int nevents = 0;
  //gSystem->ListLibraries();

  int skip_num = 0;
  Fun4AllServer *se = Fun4AllServer::instance();
  // se->Verbosity(5);
  
  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();
  
  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", CDB::global_tag);
  rc->set_uint64Flag("TIMESTAMP", CDB::timestamp);
  rc->set_IntFlag("RUNNUMBER", run_num ); // needed for the analysis modules
  
  //////////////////////////////////////////////////////////////////
  // DST input section                                           //
  //////////////////////////////////////////////////////////////////
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");
  string data = string("results/DST_physics_intt-000") + to_string(run_num) + "_no_hot.root";  
  in->fileopen( data );
  se->registerInputManager(in);
  
  //////////////////////////////////////////////////////////////////
  // Analysis moduel                                              //
  //////////////////////////////////////////////////////////////////
  // for #hit correlation
  InttTriggerTiming* itt = new InttTriggerTiming();
  itt->SetTriggerRequirement( trigger_bit );
  se->registerSubsystem( itt );
  
  //////////////////////////////////////////////////////////////////
  // Analyze!                                                     //
  //////////////////////////////////////////////////////////////////
  se->skip( skip_num );
  se->run( nevents );
  se->End();

  itt->Print();
    
  delete se;
  return 0;
}
