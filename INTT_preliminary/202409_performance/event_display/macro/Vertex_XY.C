#include "Vertex_XY.hh"

void Vertex_XY(int run_no = 41981,
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

  // Load ActsGeometry object
  TrackingInit();

  
  const string &input_file = Form("/sphenix/tg/tg01/commissioning/INTT/data/dst_files/2024/DST_physics_intt-000%d_no_hot_special.root", run_no);
  Fun4AllDstInputManager *in = new Fun4AllDstInputManager("Dst");

  auto dsts = GetDsts( run_no, false, "no_hot" );
  for( auto& dst : dsts )
    {
      in->fileopen( dst );
    }
  se->registerInputManager(in);

  InttXYVertexFinder* intt_xy = new InttXYVertexFinder();
  //  intt_xy->SetOutDirectory( "./results" );
  intt_xy->EnableQA( true );
  intt_xy->SetSaveHisto( true );
  intt_xy->EnableDrawHisto( true );
  se->registerSubsystem( intt_xy );
  
  string output = "InttAna_run" + to_string( run_no ) + ".root";
  // Fun4AllOutputManager *out = new Fun4AllDstOutputManager("out","test.root");
  // se->registerOutputManager(out);

  se->run(nEvents);

  se->End();
  delete se;
  //  gSystem->Exit(0);
  return;
}
