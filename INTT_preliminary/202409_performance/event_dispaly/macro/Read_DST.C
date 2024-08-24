#include "Read_DST.hh"

void Read_DST(int run_no = 41981
              /*const string &input_file = "/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_beam_intt-00041892_no_hot_special.root"*/,
	      int nEvents = 10)
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
  rc->set_IntFlag("RUNNUMBER", run_no );

  // TrkrHit reconstructions
  // Mvtx_Cells();
  // Load ActsGeometry object
  TrackingInit();

  // const string &input_file = Form("/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_beam_intt-000%d_special.root",run_no);
  //  const string &input_file = Form("/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_physics_intt-000%d_no_hot_special.root", run_no);
  // const string &input_file = Form("/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_beam_intt-000%d_special.root",run_no);
  Fun4AllDstInputManager *in = new Fun4AllDstInputManager("Dst");
  auto dsts = GetDsts( run_no, false, "no_hot", "temp" );
  bool is_official = true;
  for( auto& dst : dsts )
    {
      if( is_official == true && dst.find( "official" ) == string::npos )
	continue;
      
      in->fileopen( dst );
    }
  //  in->fileopen(input_file.c_str());
  se->registerInputManager(in);

  /*AnaTutorial *ana = new AnaTutorial();
  ana->analyzeTruth(true);
  ana->analyzeClusters(true);
  se->registerSubsystem(ana);*/

  InttZVertexFinder* intt_z = new InttZVertexFinder();
  //intt_z->SetOutDirectory( "./results" );
  //intt_z->EnableQA( true );
  //  intt_z->Verbosity( 0 );
  se->registerSubsystem( intt_z );

  InttXYVertexFinder* intt_xy = new InttXYVertexFinder();
  //  intt_xy->SetOutDirectory( "./results" );
  //intt_xy->EnableQA( true );
  se->registerSubsystem( intt_xy );
  
  string output = "results/InttAna_run" + to_string( run_no ) + ".root";
  InttAna *inttana = new InttAna( "InttAna", output );
  inttana->Verbosity( 2 ); // 0: minimum, 1: some, 2: detailed
  se->registerSubsystem( inttana );

  // Fun4AllOutputManager *out = new Fun4AllDstOutputManager("out","test.root");
  // se->registerOutputManager(out);

  se->run(nEvents);

  se->End();
  delete se;
  //  gSystem->Exit(0);
  return;
}
