#include "Read_DST.hh"

void Read_DST(int run_num = 50889
              /*const string &input_file = "/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_beam_intt-00041892_no_hot_special.root"*/,
	      int nEvents = 100 )
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

      dst_file = "data/DST_physics_intt-00050889_no_hot.root";
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
  se->registerSubsystem( intt_xy );

  InttZVertexFinder* intt_z = new InttZVertexFinder();
  intt_z->SetOutDirectory( "./results" );
  intt_z->EnableQA( true );
  //  intt_z->Verbosity( 0 );
  se->registerSubsystem( intt_z );
  
  string output = "results/InttAna_run" + to_string( run_num ) + ".root";
  InttAna *inttana = new InttAna( "InttAna", output );
  inttana->Verbosity( 2 ); // 0: minimum, 1: some, 2: detailed
  se->registerSubsystem( inttana );

  // Fun4AllOutputManager *out = new Fun4AllDstOutputManager("out","test.root");
  // se->registerOutputManager(out);

  if( run_num == 50889 )
    se->skip( 9117 ); // to skip the strange event
  else if( run_num == 50377 )
    se->skip( 200 ); // to skip the strange event
  
  se->run( nEvents );

  se->End();
  delete se;
  //  gSystem->Exit(0);
  return;
}
