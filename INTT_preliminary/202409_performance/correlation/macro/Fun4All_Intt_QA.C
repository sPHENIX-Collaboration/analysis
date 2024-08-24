#include "Fun4All_Intt_QA.hh"

int Fun4All_Intt_QA( int run_num = 50889,
		     int nevents = 10000,
		     bool is_official = true,
		     bool does_rawhit_qa = true,
		     bool does_trkr_hit_qa = false,
		     bool does_trkr_cluster_qa = false
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
  rc->set_IntFlag("RUNNUMBER", run_num );
  
  //////////////////////////////////////////////////////////////////
  // DST input section                                           //
  //////////////////////////////////////////////////////////////////
  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");

  // string list = "list.txt";
  // in->AddListFile( list );
  // for streaming readout data 
  if( run_num == 50889 )
    {
      if( does_trkr_hit_qa || does_trkr_cluster_qa )
	{
	  in->fileopen( "results/DST_physics_intt-00050889_no_hot.root" );
	}
      else
	{
	  // for raw hit QA
	  in->fileopen( "/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_physics_gl1_intt-00050889_special.root" );
	}
    }
  
  if( false )
    {
      string should_include = "";
      should_include = "gl1";
      string should_not_include = "no_hot";
      if( does_trkr_hit_qa || does_trkr_cluster_qa )
	{
	  // should_include = "no_hot_temp";
	  // should_not_include = "";
	  should_not_include = "temp";
	  should_include = "no_hot";
	}
      
      //  auto files = GetDsts( run_num, is_official, should_include, should_not_include );
      cout << run_num << endl;
      cout << is_official << endl;
      cout << should_include << endl;
      cout << should_not_include << endl;
      vector < string > files;
      if( does_trkr_hit_qa || does_trkr_cluster_qa )
	files = GetDstsTrkr( run_num, is_official, should_include, should_not_include );
      else 
	files = GetDsts( run_num, is_official, should_include, should_not_include );
      
      cout << "num: " << files.size() << endl;
      
      for( auto& file : files )
	cout << file << endl;
      // vector < string> files;
      // files.push_back( "/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_physics_intt-00050377_no_hot_official_temp.root" );
      
      int counter = 0;
      for( auto& file : files )
	{
	  in->fileopen( file );
	  //cout << file << endl;
	  //in->AddFile( file );
	  
	  // for official DSTs because each official DST contains few events. It's 250 events, right?
	  if( nevents > 0
	      && files.size() > 10
	      && counter * kEvent_num_per_dst > nevents )
	    break;
	  
	  counter++;
	}
    }
    
    se->registerInputManager(in);
  
  //////////////////////////////////////////////////////////////////
  // Raw hit QA                                                   //
  //////////////////////////////////////////////////////////////////
  InttRawHitQA* irhq = new InttRawHitQA( "InttRawHitQA", is_official );
  //irhq->SetOutputDir( output_dir );

  InttTrkrHitQA* trkr_hit_qa = new InttTrkrHitQA();
  //trkr_hit_qa->SetOutoutDir( "./" ); // not implemented
    
  //////////////////////////////////////////////////////////////////
  // Module registrations                                         //
  //////////////////////////////////////////////////////////////////
  if( does_rawhit_qa == true )
    {
      se->registerSubsystem( irhq );
      //se->registerSubsystem( fds );
    }

  if( does_trkr_hit_qa == true )
    {
      se->registerSubsystem( trkr_hit_qa );
      //se->registerSubsystem( fds_trkr );
      
    }

  if( does_trkr_cluster_qa == true )
    {

    }
    
  //////////////////////////////////////////////////////////////////
  // Analyze!                                                     //
  //////////////////////////////////////////////////////////////////
  se->skip( skip_num );
  se->run( nevents );
  //  cout << "Output: " << hitmap_out_file << endl;
  se->End();

  if( does_rawhit_qa == true )
    {
      irhq->Print();
      //fds->Print();
      //icrhq->Print();
    }
  
  if( does_trkr_hit_qa == true )
    {
      trkr_hit_qa->Print();
    }
  
  if( does_trkr_cluster_qa == true )
    {

    }
  
  delete se;

  return 0;
}
