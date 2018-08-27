
int Fun4All_EICAnalysis_Exclusive(
			    const int nEvents = 100,
			    const char * inputFile = "/sphenix/user/gregtom3/data/Summer2018/track2cluster_studies/sartre_20x250_dvmp_G4.root",
			    const char * outputFile = "out.root"
			    )
{

  bool readdst = true;
  bool readhepmc = false;

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libfun4all.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libeicana.so");

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

  ExclusiveReco *ana = new ExclusiveReco(outputFile);
  ana->set_do_process_dvmp( true );
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
  /* Read HepMC ASCII files  */
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
  if (nEvents == 0 && !readdst && !readhepmc)
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
