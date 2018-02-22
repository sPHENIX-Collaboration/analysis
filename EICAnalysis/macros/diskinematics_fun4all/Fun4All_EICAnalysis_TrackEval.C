/* To run this macro, need to compile the library from analysis/Tracking/FastTrackingEval */

int Fun4All_EICAnalysis_TrackEval(
				  const int nEvents = 10,
				  const char * inputFile = "/gpfs/mnt/gpfs04/sphenix/user/nfeege/sphenixsw/devel/analysis/EICAnalysis/data/G4EICDetector_DIS_SingleParticle_ele_10GeV_m3eta.root",
//				  const char * inputFile = "/gpfs/mnt/gpfs04/sphenix/user/nfeege/sphenixsw/devel/analysis/EICAnalysis/data/G4EICDetector_DIS_SingleParticle_ele_10GeV_m2eta.root",
//				  const char * inputFile = "/gpfs/mnt/gpfs04/sphenix/user/nfeege/sphenixsw/devel/analysis/EICAnalysis/data/G4EICDetector_DIS_SingleParticle_ele_10GeV_m05eta.root",
//				  const char * inputFile = "/gpfs/mnt/gpfs04/sphenix/user/nfeege/sphenixsw/devel/analysis/EICAnalysis/data/G4EICDetector_DIS_SingleParticle_ele_10GeV_p05eta.root",
//				  const char * inputFile = "/gpfs/mnt/gpfs04/sphenix/user/nfeege/sphenixsw/devel/analysis/EICAnalysis/data/G4EICDetector_DIS_SingleParticle_ele_10GeV_p2eta.root",
				  const char* evalFile = "eval_track_fastsim.root"
				  )
{

  bool readdst = true;
  bool readhepmc = false;

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libeicana.so");
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4eval.so");

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);
  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();

  //--------------
  // Additional tracking modules
  //--------------

  gROOT->LoadMacro("G4_FastSim_Reco_EIC.C");
  FastSim_Reco_EIC();

  //--------------
  // Analysis modules
  //--------------

  gSystem->Load("libFastTrackingEval.so");
  FastTrackingEval* eval_fasttrack;
  eval_fasttrack = new FastTrackingEval("FASTTRACKEVALUATOR", evalFile, "SvtxTrackMap_FastSim");
  se->registerSubsystem( eval_fasttrack );

  //SvtxEvaluator* eval;
  //eval = new SvtxEvaluator("SVTXEVALUATOR", "track_eval.root", "SvtxTrackMap_FastSim");
  ////eval = new SvtxEvaluator("SVTXEVALUATOR", "track_eval.root", "SvtxTrackMap");
  //eval->do_cluster_eval(true);
  //eval->do_g4hit_eval(true);
  //eval->do_hit_eval(false);  // enable to see the hits that includes the chamber physics...
  //eval->do_gpoint_eval(false);
  //eval->scan_for_embedded(false);  // take all tracks if false - take only embedded tracks if true
  //eval->Verbosity(0);
  //se->registerSubsystem(eval);

  //--------------
  // IO management
  //--------------

  /* Read DST Files */
  if ( readdst )
    {
      Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
      hitsin->fileopen(inputFile);
      se->registerInputManager(hitsin);
    }
  /* Read HepMC ASCII files */
  else if ( readhepmc )
    {
      Fun4AllInputManager *in = new Fun4AllHepMCInputManager( "DSTIN");
      se->registerInputManager( in );
      se->fileopen( in->Name().c_str(), inputFile );
    }


  //-----------------
  // Event processing
  //-----------------
  if (nEvents < 0)
    {
      return;
    }
  // if we run the particle generator and use 0 it'll run forever
  if (nEvents == 0 && !readhits && !readhepmc)
    {
      cout << "using 0 for number of events is a bad idea when using particle generators" << endl;
      cout << "it will run forever, so I just return without running anything" << endl;
      return;
    }

  se->run(nEvents);

  //-----
  // Exit
  //-----

  se->End();
  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
}
