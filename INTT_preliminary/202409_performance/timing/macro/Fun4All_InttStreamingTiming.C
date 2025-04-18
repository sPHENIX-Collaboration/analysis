#include "Fun4All_InttStreamingTiming.hh"

int Fun4All_InttStreamingTiming( int trigger_bit = -1 ) // -1 means no requirement
{

  int run_num = 50889;
  int nevents = 100000;
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
  in->fileopen( "results/DST_physics_intt-00050889_no_hot.root" );  
  se->registerInputManager(in);
  
  //////////////////////////////////////////////////////////////////
  // Analysis moduel                                              //
  //////////////////////////////////////////////////////////////////
  // for #hit correlation
  InttStreamingTiming* ist = new InttStreamingTiming();
  ist->SetTriggerRequirement( trigger_bit );
  ist->SetMaxEvent( nevents );
  se->registerSubsystem( ist );
  
  //////////////////////////////////////////////////////////////////
  // Analyze!                                                     //
  //////////////////////////////////////////////////////////////////
  se->skip( skip_num );
  se->run( nevents );
  se->End();

  ist->Print();
    
  delete se;
  return 0;
}
