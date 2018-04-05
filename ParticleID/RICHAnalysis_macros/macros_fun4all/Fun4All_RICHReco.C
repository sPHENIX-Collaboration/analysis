
int Fun4All_RICHReco(
                     const int nEvents = 1,
                     const char * inputFile = "G4EICDetector.root",
                     const char * evalFile = "eval_RICH.root"
                     )
{

  bool readdst = true;

  /* switch: do_fastpid = 'true' uses FastPid module based on parametrization of RICH response; if
     set to 'false', use full reconstructed based on detected photon hits. */
  bool do_fastpid = false;

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("librichana.so");
  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");

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

  /* Adding RICH analysis modules */
  if ( do_fastpid )
    {
      FastPid_RICH *fastpid_rich = new FastPid_RICH("SvtxTrackMap", "RICH");
      se->registerSubsystem(fastpid_rich);
    }
  else
    {
      RICHParticleID *richpid = new RICHParticleID("SvtxTrackMap", "RICH");
      richpid->set_refractive_index(1.000526);
      se->registerSubsystem(richpid);

      RICHEvaluator *richeval = new RICHEvaluator("SvtxTrackMap", "RICH", evalFile);
      richeval->set_refractive_index(1.000526);
      se->registerSubsystem(richeval);
    }

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
