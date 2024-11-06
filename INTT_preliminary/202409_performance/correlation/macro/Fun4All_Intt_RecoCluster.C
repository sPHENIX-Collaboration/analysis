#include "Fun4All_Intt_RecoCluster.hh"

void Fun4All_Intt_RecoCluster( int run_num = 50889,
			       int fphx_bco = -1
			       )
{

  /////////////////////////////////////////////////
  // Run 50377: triggered only                   //
  // Run 50379: triggered + extended             //
  // Run 50889: streaming (FPHX BCO cut applied) //
  /////////////////////////////////////////////////
  
  //  gSystem->ListLibraries();
  TStopwatch* watch = new TStopwatch();
  watch->Start();
  
  ////////////////////////////////////////////////////////////////////////

  int nevents = 100000;
  string run_type = "physics";
  string run_num_str = string( "000" ) + to_string( run_num );

  string list_file = string( "list_run" ) + to_string( run_num ) + ".txt";
  
  ////////////////////////////////////////////////////////////////////////
  // Config for input/output files
  ////////////////////////////////////////////////////////////////////////

  // output_base format: {run_tpye}_intt_{run number}
  // For example, cosmics_intt_01234567
  string output_base = string("_") + run_type + "_intt-" + run_num_str;  
  string output_dst = string("results/") + "DST" + output_base;

  output_dst += "_no_hot";
  if( run_num == 50889 )
    output_dst += "_FPHX_BCO_" + to_string( fphx_bco );
  
  output_dst +=  ".root"; 

  // string  cdb_hot_list = kIntt_cdb_dir + "cdb_49737_special.root";
  // cdb_hot_list = kIntt_cdb_dir + "cdb_00050377_special.root";
  // cout << "Hot channel CDB is forced to be " << cdb_hot_list  << endl;
  string  cdb_hot_list = string( "data/hotmap_cdb/hotmap_run_000" ) + to_string( run_num ) + ".root";

  string cdb_bco = kIntt_cdb_dir + "bco_diff_map/"
    + "cdb_bco_" + run_num_str + "_official" + ".root";

  string cdbtree_name_dac = kIntt_cdb_dir
    + "dac_map/"
    //  + "cdb_intt_dac_30_45_60_90_120_150_180_210.root";
    + "cdb_intt_dac_35_45_60_90_120_150_180_210_streaming.root";
  
  Fun4AllServer* se = Fun4AllServer::instance();
  //se->Verbosity(5);

  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  rc->set_IntFlag("RUNNUMBER", run_num );
  
  //--input
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST");
  in->Verbosity(2);
  in->AddListFile( list_file ); 
  se->registerInputManager(in);
     
  //Enable::BBC = true;
  Enable::MVTX = true;
  Enable::INTT = true;
  Enable::TPC = true;
  Enable::MICROMEGAS = true;
  G4Init();
  G4Setup();

  TrackingInit();

  //////////////////////////////////////
  InttCombinedRawDataDecoder* inttdecode = new InttCombinedRawDataDecoder();
  //  inttdecode->Verbosity(1);
  inttdecode->runInttStandalone( true );
  inttdecode->writeInttEventHeader( true );

  inttdecode->LoadHotChannelMapLocal( cdb_hot_list );

  if( run_num == 50377 ) // BCO diff selection to get only hits from triggered collision
    inttdecode->SetCalibBCO( cdb_bco, InttCombinedRawDataDecoder::FILE); 

  inttdecode->SetCalibDAC( cdbtree_name_dac, InttCombinedRawDataDecoder::FILE ); // not InttCombinedRawDataDecoder::CDB

  if( run_num == 50889 )
    inttdecode->set_fphxBcoFilter( fphx_bco );
  else
    inttdecode->set_fphxBcoFilter( -1 );
  
  se->registerSubsystem( inttdecode );
  
  //////////////////////////////////////
  //Intt_Cells();
  Intt_Clustering();

  //output
  Fun4AllOutputManager* out = new Fun4AllDstOutputManager("DST", output_dst );
  se->registerOutputManager(out);

  se->run(nevents);
  se->End();

  cout << "CDB (hot channel) " << cdb_hot_list << endl;
  cout << "CDB (BCO diff) " << cdb_bco << endl;
  cout << "CDB (DAC map)    " << cdbtree_name_dac << endl;
  cout << "FPHX BCO: " << fphx_bco << endl;
  cout << "Output: " << output_dst << endl;

  watch->Stop();
  cout << "----------------------------------" << endl;
  cout << "Real time: " << watch->RealTime() << endl;
  cout << "CPU time:  " << watch->CpuTime() << endl;  
  delete se;
}
