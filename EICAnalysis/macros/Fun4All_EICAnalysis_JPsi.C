
int Fun4All_EICAnalysis_Quarkonia(
                          const int nEvents = 100000,
                          //const char * inputFile = "../data/phpythia6_e20p250_jpsi_jpsi2ee_100k_hepmc.root",
                          //const char * outputFile = "eicmcana_pythia6_e20p250_jpsi_jpsi2ee_100k.root"
                          //const char * inputFile = "../data/phpythia6_e10p250_jpsi_jpsi2ee_100k_hepmc.root",
                          //const char * outputFile = "eicmcana_pythia6_e10p250_jpsi_jpsi2ee_100k.root"
                          //const char * inputFile = "../data/phpythia6_e10p100_jpsi_jpsi2ee_100k_hepmc.root",
                          //const char * outputFile = "eicmcana_pythia6_e10p100_jpsi_jpsi2ee_100k.root"
                          const char * inputFile = "../data/phpythia6_e20p250_ccbar_jpsi2ee_1k_hepmc.root",
                          const char * outputFile = "eicmcana_pythia6_e20p250_ccbar_jpsi2ee_1k.root"
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

  //  rc->set_FloatFlag("WorldSizex",1000);
  //  rc->set_FloatFlag("WorldSizey",1000);
  //  rc->set_FloatFlag("WorldSizez",1000);
  //  rc->set_CharFlag("WorldShape","G4Tubs");


  //--------------
  // Analysis modules
  //--------------

  //  DISKinematics *disana = new DISKinematics("testdis.root");
  //  se->registerSubsystem( disana );

  Quarkonia2LeptonsMC *mcana = new Quarkonia2LeptonsMC(outputFile);
  se->registerSubsystem( mcana );

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
