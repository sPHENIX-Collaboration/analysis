void ConfigureInput(TString inputFile) {
  const bool hijing = true;
  const bool genera = true;

  Fun4AllServer *se = Fun4AllServer::instance();

  if(hijing) {
    HepMCNodeReader *hr = new HepMCNodeReader();
    //hr->SmearVertex(0.0,0.0,-5.0); // optional
    se->registerSubsystem(hr);
    Fun4AllInputManager *in = new Fun4AllHepMCInputManager( "DSTIN" );
    se->registerInputManager( in );
    in->fileopen( inputFile.Data() ); //fileopen only after you register!
  }

  if(genera) {
    PHG4SimpleEventGenerator *gen = new PHG4SimpleEventGenerator( "PIONS" );
    gen->add_particles("pi-",5); // mu+,e+,proton,pi+,Upsilon
    gen->add_particles("pi+",5); // mu-,e-,anti_proton,pi-
    if(hijing) {
      gen->set_reuse_existing_vertex(true);
      gen->set_existing_vertex_offset_vector(0.0, 0.0, 0.0);
    } else {
      gen->set_vertex_distribution_function(PHG4SimpleEventGenerator::Uniform,
					    PHG4SimpleEventGenerator::Uniform,
					    PHG4SimpleEventGenerator::Uniform);
      gen->set_vertex_distribution_mean(0.0, 0.0, 0.0);
      gen->set_vertex_distribution_width(0.0, 0.0, 0.0);
    }
    gen->set_vertex_size_function(PHG4SimpleEventGenerator::Uniform);
    gen->set_vertex_size_parameters(0.0, 0.0);
    gen->set_eta_range(-1.1,+1.1);
    gen->set_phi_range(-1.0 * TMath::Pi(), 1.0 * TMath::Pi());
    gen->set_pt_range(0.2, 39.0);
    gen->Embed(1);
    gen->Verbosity(0);

    se->registerSubsystem(gen);
  }
}
void RandomSeed() {
  // By default every random number generator uses
  // PHRandomSeed() which reads /dev/urandom to get its seed
  // if the RANDOMSEED flag is set its value is taken as seed
  // You ca neither set this to a random value using PHRandomSeed()
  // which will make all seeds identical (not sure what the point of
  // this would be:
  // or set it to a fixed value so you can debug your code

  recoConsts *rc = recoConsts::instance();
  //  rc->set_IntFlag("RANDOMSEED",PHRandomSeed());
  //  rc->set_IntFlag("RANDOMSEED", 12345);
}
void ConfigureDetectors() {
  bool do_magnet = true;
  bool do_svtx = true;
  bool do_preshower = false;
  bool do_cemc = false;
  bool do_hcalin = false;
  bool do_hcalout = false;
  bool do_pipe = true;
  G4Init(do_svtx,do_preshower,do_cemc,do_hcalin,do_magnet,do_hcalout,do_pipe);

  int absorberactive = 1; // set to 1 to make all absorbers active volumes
  //  const string magfield = "1.5"; // if like float -> solenoidal field in T, if string use as fieldmap name (including path)
  const string magfield = "/phenix/upgrades/decadal/fieldmaps/sPHENIX.2d.root"; // if like float -> solenoidal field in T, if string use as fieldmap name (including path)
  const float magfield_rescale = 1.4/1.5; // scale the map to a 1.4 T field

  bool do_svtx_cell = do_svtx && true;
  bool do_svtx_track = do_svtx_cell && true;
  bool do_svtx_eval = do_svtx_track && false;

  bool do_bbc = true;
  bool do_cemc_cell = do_cemc && true;
  bool do_cemc_twr = do_cemc_cell && true;
  bool do_cemc_cluster = do_cemc_twr && true;
  bool do_cemc_eval = do_cemc_cluster && true;
  bool do_hcalin_cell = do_hcalin && true;
  bool do_hcalin_twr = do_hcalin_cell && true;
  bool do_hcalin_cluster = do_hcalin_twr && true;
  bool do_hcalin_eval = do_hcalin_cluster && true;
  bool do_hcalout_cell = do_hcalout && true;
  bool do_hcalout_twr = do_hcalout_cell && true;
  bool do_hcalout_cluster = do_hcalout_twr && true;
  bool do_hcalout_eval = do_hcalout_cluster && true;
  bool do_global = true;
  bool do_global_fastsim = true;
  bool do_jet_reco = false;
  bool do_jet_eval = false;
  bool do_dst_compress = false;
  bool do_DSTReader = false;
 
  G4Setup(absorberactive, magfield, TPythia6Decayer::kAll,
	  do_svtx, do_preshower, do_cemc, do_hcalin, do_magnet,
	  do_hcalout, do_pipe, magfield_rescale);
  if (do_bbc) {
    gROOT->LoadMacro("G4_Bbc.C");
    BbcInit();
    Bbc_Reco();
  }
  if (do_svtx_cell) Svtx_Cells();
  if (do_cemc_cell) CEMC_Cells();
  if (do_hcalin_cell) HCALInner_Cells();
  if (do_hcalout_cell) HCALOuter_Cells();
  if (do_cemc_twr) CEMC_Towers();
  if (do_cemc_cluster) CEMC_Clusters();
  if (do_hcalin_twr) HCALInner_Towers();
  if (do_hcalin_cluster) HCALInner_Clusters();
  if (do_hcalout_twr) HCALOuter_Towers();
  if (do_hcalout_cluster) HCALOuter_Clusters();
  if (do_dst_compress) ShowerCompress();
  if (do_svtx_track) Svtx_Reco();
  if (do_global)  {
    gROOT->LoadMacro("G4_Global.C");
    Global_Reco();
  } else if (do_global_fastsim) {
    gROOT->LoadMacro("G4_Global.C");
    Global_FastSim();
  }
  if (do_jet_reco) {
    gROOT->LoadMacro("G4_Jets.C");
    Jet_Reco();
  }
  if (do_svtx_eval) Svtx_Eval("g4svtx_eval.root");
  if (do_cemc_eval) CEMC_Eval("g4cemc_eval.root");
  if (do_hcalin_eval) HCALInner_Eval("g4hcalin_eval.root");
  if (do_hcalout_eval) HCALOuter_Eval("g4hcalout_eval.root");
  if (do_jet_eval) Jet_Eval("g4jet_eval.root");

}
void Libraries() {
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");
  gSystem->Load("libg4calo.so");
  gSystem->Load("libg4eval.so");
  gSystem->Load("libTrackingPerformanceCheck.so");
  gROOT->LoadMacro("G4Setup_sPHENIX.C");
}

int Fun( const int nEvents = 1,
	 const int nFile = 0 ) {
  if(nEvents < 0) return;
  TStopwatch time;
  time.Start();
  Libraries();

  TString inputFile = Form("/gpfs/mnt/gpfs02/phenix/hhj/hhj1/frawley/tracking/stage1_jobs/in/hijing_%05d.txt.bz2",nFile);
  //TString outputFile = Form("output/output_%05d.root",nFile);
  TString outputFile = Form("output_%05d.root",nFile);

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  RandomSeed();
  ConfigureInput(inputFile);
  ConfigureDetectors();

  //ADDTASKS
  TrackingPerformanceCheck *revperf = new TrackingPerformanceCheck();
  revperf->SetLayerTPCBegins(7);
  revperf->SetReconstructableTPCHits(30);
  revperf->SetFairClustersContribution(20);
  revperf->SetGoodTrackChi2NDF(2.0);
  revperf->SetGoodTrackTPCClusters(20);
  se->registerSubsystem(revperf);

  std::cout << "RUNNING..." << std::endl;
  //se->Print();
  se->run(nEvents);
  se->dumpHistos(outputFile.Data());
  se->End();
  std::cout << "ALL DONE" << std::endl;
  time.Stop();
  time.Print();
  delete se;
  gSystem->Exit(0);
}
