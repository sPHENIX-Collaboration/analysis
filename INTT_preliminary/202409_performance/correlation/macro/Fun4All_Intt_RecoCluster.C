#include "Fun4All_Intt_RecoCluster.hh"

void Fun4All_Intt_RecoCluster( )
  // int run_num,
  // 			       int nevents=10,
  // 			       bool is_official = true, 
  // 			       bool use_cdb = true
  // 			       )
{

  //  gSystem->ListLibraries();
  

  ////////////////////////////////////////////////////////////////////////
  int run_num = 50889;
  int nevents = 100000;
  string run_type = "physics";
  string run_num_str = string( "000" ) + to_string( run_num );

  
  ////////////////////////////////////////////////////////////////////////
  // Config for input/output files
  ////////////////////////////////////////////////////////////////////////
  // string run_type = GetRunType( run_num );
  // string run_num_str = GetRunNum8digits( run_num );

  // output_base format: {run_tpye}_intt_{run number}
  // For example, cosmics_intt_01234567
  string output_base = string("_") + run_type + "_intt-" + run_num_str;  
  string output_dst = string("results/") + "DST" + output_base;

  output_dst += "_no_hot";

  output_dst +=  ".root"; 

  string  cdb_hot_list = kIntt_cdb_dir + "cdb_49737_special.root";
  cdb_hot_list = kIntt_cdb_dir + "cdb_00050377_special.root";
  cout << "Hot channel CDB is forced to be " << cdb_hot_list  << endl;

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
  // global tag
  rc->set_StringFlag("CDB_GLOBALTAG",CDB::global_tag);
  // cout << "CDB_GLOBALTAG : " << CDB::global_tag << endl;
  // cout << "CDB_GLOBALTAG : " << rc->get_StringFlag("CDB_GLOBALTAG") << endl;

  // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP",CDB::timestamp);
  //cout<<"TIMESTAMP : "<<CDB::timestamp<<endl;
  //rc->set_uint64Flag("TIMESTAMP",20869);
  //cout<<"TIMESTAMP : "<<rc->get_uint64Flag("TIMESTAMP")<<endl;
  rc->set_IntFlag("RUNNUMBER", run_num );
  
  //--input
  //  auto files = GetDsts( run_num, is_official, "", "no_hot" );
  string dst_streaming = "/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_physics_gl1_intt-00050889_special.root";
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST");
  in->fileopen( dst_streaming );
  
  in->Verbosity(2);
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

  //inttdecode->SetCalibBCO( cdb_bco, InttCombinedRawDataDecoder::FILE);

  //inttdecode->SetCalibDAC("CDBTTree_INTT_DACMAP.root", InttCombinedRawDataDecoder::FILE);
  inttdecode->SetCalibDAC( cdbtree_name_dac, InttCombinedRawDataDecoder::FILE ); // not InttCombinedRawDataDecoder::CDB
  //  inttdecode->SetBCODiffSelectionTolerance( 0 );
  se->registerSubsystem( inttdecode );
  
  //////////////////////////////////////
  //Intt_Cells();
  //Intt_Clustering( is_z_clustering );
  Intt_Clustering();

  //output
  Fun4AllOutputManager* out = new Fun4AllDstOutputManager("DST", output_dst );
  se->registerOutputManager(out);

  se->run(nevents);
  se->End();

  cout << "CDB (hot channel) " << cdb_hot_list << endl;
  cout << "CDB (BCO diff) " << cdb_bco << endl;
  cout << "CDB (DAC map)    " << cdbtree_name_dac << endl;
  cout << "Output: " << output_dst << endl;

  delete se;

}
