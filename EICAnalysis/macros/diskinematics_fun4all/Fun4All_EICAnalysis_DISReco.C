
int Fun4All_EICAnalysis_DISReco(
				const int nEvents = 1000,
				const char * inputFile = "/gpfs/mnt/gpfs02/phenix/scratch/spjeffas/g4sim/G4_Leptoquark_DST_p250_e20_1000events_1seed_DIScharged.root",
				//const char * inputFile = "G4EICDetector_DIS_10x250_20events.root",
				//const char * inputFile = "G4EICDetector_DIS_SingleParticle_ele_10GeV_m05eta.root",
				//const char * inputFile = "G4EICDetector_DIS_SingleParticle_ele_10GeV_m2eta.root",
				const char * outputFile = "eicana_disreco_test2.root"
				)
{

  bool readdst = true;
  bool readhepmc = false;

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libeicana.so");
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  //  gSystem->Load("libg4testbench.so");
  //  gSystem->Load("libg4hough.so");
  //  gSystem->Load("libcemc.so");
  gSystem->Load("libg4bbc.so");
  gSystem->Load("libg4eval.so");

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);
  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();

  //--------------
  // Analysis modules
  //--------------

  gROOT->LoadMacro("G4_FastSim_Reco_EIC.C");
  FastSim_Reco_EIC();

  DISKinematicsReco *ana = new DISKinematicsReco(outputFile);
  ana->set_do_process_truth( true );
  ana->set_do_process_geant4_cluster( true );
  se->registerSubsystem( ana );

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
