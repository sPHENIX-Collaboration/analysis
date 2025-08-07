#include "Fun4All_Intt_Correlation.hh"

int Fun4All_Intt_Correlation( int run_num = 50889,
			      int nevents = 10000,
			      bool does_hit_correlation = true,
			      bool does_cluster_correlation = true,
			      int fphx_bco = -1
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
      string data = "results/DST_physics_intt-00050889_no_hot";
      if( fphx_bco != 9999 )
	data += "_FPHX_BCO_" + to_string( fphx_bco ) + ".root";
      else
	data += ".root";

      in->fileopen( data );
    }
  else if( run_num == 50377 ) // triggered mode
    {
      //in->fileopen( "results/DST_physics_gl1_intt-00050377_no_hot.root" );
      in->fileopen( "results/DST_physics_intt-00050377_no_hot.root" );
      skip_num = 150000;
    }
  
  se->registerInputManager(in);
  
  //////////////////////////////////////////////////////////////////
  // Analysis moduel                                              //
  //////////////////////////////////////////////////////////////////
  // for #hit correlation
  InttHitCorrelation* hit_cor = new InttHitCorrelation();
  hit_cor->SetFphxBco( fphx_bco );
  if( does_hit_correlation == true )
    se->registerSubsystem( hit_cor );
  
  // for #cluster correlation
  InttClusterCorrelation* cluster_cor = new InttClusterCorrelation();
  cluster_cor->SetFphxBco( fphx_bco );
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
