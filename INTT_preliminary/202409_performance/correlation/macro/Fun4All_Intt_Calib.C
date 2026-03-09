#include "Fun4All_Intt_Calib.hh"

void Fun4All_Intt_Calib(int run_num = 50889,
			 int nevents = 1000,
			 bool is_official = true
			 )
{
  Fun4AllServer *se = Fun4AllServer::instance();
  // se->Verbosity(5);
  
  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();
  
  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", CDB::global_tag);
  rc->set_uint64Flag("TIMESTAMP", CDB::timestamp);
  rc->set_IntFlag("RUNNUMBER", run_num );
  
  //////////////////////////////////////////////////////////////////
  // DST input section                                           //
  //////////////////////////////////////////////////////////////////
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");

  //auto files = GetDsts( run_num, is_official, "", "no_hot" ); // remove files with no_hot in their name
  auto files = GetDsts( run_num, is_official, "gl1", "no_hot" ); // remove files with no_hot in their name
  int counter = 0;
  for( auto& file : files )
    {
      //in->fileopen( file );
      in->AddFile( file );

      // for official DSTs because each official DST contains few events. It's 250 events, right?
      if( is_official == true
	  && nevents > 0
	  && files.size() > 10
	  && counter * kEvent_num_per_dst > nevents )
	break;

      //cout << counter * kEvent_num_per_dst << endl;
      counter++;
    }
  
  se->registerInputManager(in);
  
  //////////////////////////////////////////////////////////////////
  // Path to the output hitmap (.root)                            //
  //////////////////////////////////////////////////////////////////
  // std::string hitmap_out_file = kIntt_hitmap_dir + "root/"
  //   + "hitmap_run" + GetRunNum8digits(run_num);
  // if( is_official )
  //   hitmap_out_file += "_official";
  // else
  //   hitmap_out_file += "_special"; // good to avoid using "private" for ourself...
      
  // hitmap_out_file += ".root";

  string suffix_official_private = "_official";
  if( is_official == false )
    suffix_official_private += "_special"; // good to avoid using "private" for ourself...
  
  //////////////////////////////////////////////////////////////////
  // InttCalib module                                             //
  //////////////////////////////////////////////////////////////////
  InttCalib* calib = new InttCalib();
  //  calib->m_NUM_SIGMA = 5.0;
  double sigma = 5.0;
  
  string run_num_str = GetRunNum8digits( run_num );
  string hot_ch_cdb_file = kIntt_hot_ch_cdb_dir + "cdb_" + run_num_str + suffix_official_private + ".root";
  string hot_ch_png_file = kIntt_qa_hotmap_dir + "plots/" + to_string( kYear ) + "/"
    + "InttHotDeadMap_" + run_num_str + "_" + std::to_string( int(10*sigma) ) + ".png";

  // calib->SetHotMapCdbFile( "./cdb_test_hot_map.root" );
  // calib->SetHotMapPngFile( "./cdb_test_hot_map.png" );
  calib->SetHotMapCdbFile( hot_ch_cdb_file );
  calib->SetHotMapPngFile( hot_ch_png_file );

  string bco_diff_cdb_file = kIntt_bco_diff_cdb_dir + "cdb_bco_" + run_num_str + suffix_official_private + ".root";
  string bco_diff_png_file = kIntt_qa_bco_diff_dir + "plots/" + to_string( kYear ) + "/"
    + "bco_" + run_num_str + ".png";
  // calib->SetBcoMapCdbFile( "./cdb_test_bco_map.root" );
  // calib->SetBcoMapPngFile( "./cdb_test_bco_map.png" );
  calib->SetBcoMapCdbFile( bco_diff_cdb_file );
  calib->SetBcoMapPngFile( bco_diff_png_file );
  se->registerSubsystem( calib );
  
  //////////////////////////////////////////////////////////////////
  // Analyze!                                                     //
  //////////////////////////////////////////////////////////////////
  se->run(nevents);
  //cout << "Output: " << hitmap_out_file << endl;
  se->End();

  cout << "Output (hot ch,   CDB): " << hot_ch_cdb_file << endl;
  cout << "Output (hot ch,   png): " << hot_ch_png_file << endl;  
  cout << "Output (bco diff, CDB): " << bco_diff_cdb_file << endl;
  cout << "Output (bco diff, png): " << bco_diff_png_file << endl;
  //calib->SaveHitrates(); // Don't use it
  
  delete se;
}
