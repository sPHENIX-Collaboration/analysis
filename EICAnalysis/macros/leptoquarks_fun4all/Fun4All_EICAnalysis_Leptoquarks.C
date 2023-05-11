
int Fun4All_EICAnalysis_Leptoquarks(
				    const int nEvents = 10,
				    const char * inputFile = "/gpfs/mnt/gpfs02/phenix/scratch/jlab/Leptoquark/TestOut.10event.root",
				    const char * outputFile = "eicana_milou.root"
				    )
{

  const bool readdst = false;
  const bool readhepmc = false;
  const bool readeictree = true;

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libfun4all.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");
  gSystem->Load("libg4calo.so");
  gSystem->Load("libg4eval.so");
  gSystem->Load("libeicana.so");

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);
  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();

  rc->set_FloatFlag("WorldSizex",1000);
  rc->set_FloatFlag("WorldSizey",1000);
  rc->set_FloatFlag("WorldSizez",1000);
  rc->set_CharFlag("WorldShape","G4Tubs");


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
  /* Read EICTree files */
  else if (readeictree)
    {
      // this module is needed to read the EICTree style records into our G4 sims
      ReadEICFiles *eicr = new ReadEICFiles();
      eicr->OpenInputFile(inputFile);
      se->registerSubsystem(eicr);
    }

  //--------------
  // Analysis modules
  //--------------

  Leptoquarks *mcana = new Leptoquarks(outputFile);
  mcana->set_beam_energies(10,250);
  se->registerSubsystem( mcana );


  //-----------------
  // Event processing
  //-----------------
  if (nEvents < 0)
    {
      return;
    }
  // if we run the particle generator and use 0 it'll run forever
  if (nEvents == 0 && !readhits && !readhepmc && !readeictree)
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
