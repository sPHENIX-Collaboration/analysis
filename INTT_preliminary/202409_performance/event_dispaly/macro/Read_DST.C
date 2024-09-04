#include "Read_DST.hh"

void Read_DST(int run_num = 50889
              /*const string &input_file = "/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_beam_intt-00041892_no_hot_special.root"*/,
	      int event_num = 100,
	      int fphx_bco = 63 )
{
  //gSystem->ListLibraries();

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);
  recoConsts *rc = recoConsts::instance();

  //Enable::MVTX = true;
  Enable::INTT = true;

  //===============
  // conditions DB flags
  //===============
  Enable::CDB = true;
  // global tag
  rc->set_StringFlag("CDB_GLOBALTAG", CDB::global_tag);
  // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP", CDB::timestamp);
  rc->set_IntFlag("RUNNUMBER", run_num );

  // TrkrHit reconstructions
  // Mvtx_Cells();
  // Load ActsGeometry object
  TrackingInit();

  // const string &input_file = Form("/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_beam_intt-000%d_special.root",run_num);
  //  const string &input_file = Form("/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_physics_intt-000%d_no_hot_special.root", run_num);
  // const string &input_file = Form("/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_beam_intt-000%d_special.root",run_num);
  Fun4AllDstInputManager *in = new Fun4AllDstInputManager("Dst");

  string dst_file = "";
  if( run_num == 50889 ) // steaming mode
    {
      // DST_physics_intt-00050889_no_hot_FPHX_BCO_100.root
      dst_file = string("data/DST_physics_intt-00050889_no_hot_FPHX_BCO_") + to_string(fphx_bco) + ".root";
    }
  else if( run_num == 50377 ) // triggered mode
    {
      dst_file = "data/DST_physics_intt-00050377_no_hot.root";
    }

  in->fileopen( dst_file );  
  se->registerInputManager(in);

  InttXYVertexFinder* intt_xy = new InttXYVertexFinder();
  //  intt_xy->SetOutDirectory( "./results" ); // not implemented yet
  intt_xy->EnableQA( true );
  intt_xy->SetBeamCenter(-0.0188185, 0.198181);
  se->registerSubsystem( intt_xy );

  InttZVertexFinder* intt_z = new InttZVertexFinder();
  intt_z->SetOutDirectory( "./results" );
  intt_z->EnableQA( true );
  intt_z->SetBeamCenter(-0.0188185, 0.198181);  // 1st 10k version
  //  intt_z->Verbosity( 0 );
  se->registerSubsystem( intt_z );
  
  string output = "results/InttAna_run" + to_string( run_num ) + "_FPHX_BCO_" + to_string( fphx_bco ) + ".root";
  InttAna *inttana = new InttAna( "InttAna", output );
  inttana->Verbosity( 0 ); // 0: minimum, 1: some, 2: detailed
  //inttana->SetBeamCenter(-0.0188185, 0.198181);  // 1st 10k version
  se->registerSubsystem( inttana );

  // Fun4AllOutputManager *out = new Fun4AllDstOutputManager("out","test.root");
  // se->registerOutputManager(out);

  if( run_num == 50889 )
    {
      //   0: not work
      //   5: not work
      //  10: good
      //  15: good
      //  20: 
      //  25:
      //  30:
      //  35:
      //  40:
      //  45:
      //  50

      //  80:
      //  85:
      //  90:
      //  95:
      // 100:
      // 105: 
      // 110: good
      // 115: by skipping ~27000 events, 72385 events can be processed
      // 120: good
      if( fphx_bco == 20 )
	{
	  if( event_num == 0 )
	    event_num = 73426;

	  se->skip( 5074 );
	}
      else if( fphx_bco == 63 )
	{
	  //se->skip( 9117 ); // to skip the strange event
	  se->skip( 9117 + 5303 ); // to skip the strange event
	}
      else if( fphx_bco == 105 )
	se->skip( 80304 );
      else if( fphx_bco == 115 )
	se->skip( 13489 + 14125 );
      
    }
  else if( run_num == 50377 )
    {
      se->skip( 200 ); // to skip the strange event
    }
  
  se->run( event_num );

  se->End();
  delete se;
  //  gSystem->Exit(0);
  return;
}
