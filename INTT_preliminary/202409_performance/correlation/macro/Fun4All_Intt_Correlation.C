#include "Fun4All_Intt_Correlation.hh"

int Fun4All_Intt_Correlation( int run_num = 50889,
			      int nevents = 10000,
			      bool does_hit_correlation = true,
			      bool does_cluster_correlation = true
			      )
{

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

  if( run_num == 50889 ) // streaming readout mode
    {
      in->fileopen( "results/DST_physics_intt-00050889_no_hot.root" );
    }
  else if( run_num == 50377 ) // triggered mode
    {
      in->fileopen( "results/DST_physics_intt-00050377_no_hot.root" );
    }
  
  se->registerInputManager(in);
  
  //////////////////////////////////////////////////////////////////
  // Analysis moduel                                              //
  //////////////////////////////////////////////////////////////////
  // for #hit correlation
  InttHitCorrelation* hit_cor = new InttHitCorrelation();
  if( does_hit_correlation == true )
    se->registerSubsystem( hit_cor );
  
  // for #cluster correlation
  InttClusterCorrelation* cluster_cor = new InttClusterCorrelation();
  if( does_cluster_correlation == true )
    se->registerSubsystem( cluster_cor );
  
  //////////////////////////////////////////////////////////////////
  // Analyze!                                                     //
  //////////////////////////////////////////////////////////////////
  se->skip( skip_num );
  se->run( nevents );
  se->End();

  if( does_hit_correlation == true )
    hit_cor->Print();
  
  if( does_cluster_correlation == true )
    cluster_cor->Print();
  
  delete se;
  return 0;
}
