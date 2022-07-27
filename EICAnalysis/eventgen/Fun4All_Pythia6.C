int Fun4All_Pythia6(
                    const int nEvents = 30,
                    const char * pythiaConfigFile = "./config/phpythia6_ep_DISneutral.cfg",
                    const char * outputFile = "DST_p250_e20_1seed_DISneutral.root"
                    )
{

  bool runpythia6 = true;

  bool writedst = false;
  bool writeascii = true;

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
  // By default every random number generator uses
  // PHRandomSeed() which reads /dev/urandom to get its seed
  // if the RANDOMSEED flag is set its value is taken as seed
  // You ca neither set this to a random value using PHRandomSeed()
  // which will make all seeds identical (not sure what the point of
  // this would be:
  //  rc->set_IntFlag("RANDOMSEED",PHRandomSeed());
  // or set it to a fixed value so you can debug your code
  //  rc->set_IntFlag("RANDOMSEED", 12345);

  rc->set_FloatFlag("WorldSizex",1000);
  rc->set_FloatFlag("WorldSizey",1000);
  rc->set_FloatFlag("WorldSizez",1000);
  rc->set_CharFlag("WorldShape","G4Tubs");

  //-----------------
  // Event generation
  //-----------------

  if (runpythia6)
    {
      gSystem->Load("libPHPythia6.so");

      PHPythia6 *pythia6 = new PHPythia6();
      pythia6->set_config_file(pythiaConfigFile);
      
      PHPy6ParticleTrigger *trigger = new PHPy6ParticleTrigger();
      //trigger->SetParticleType(15);
      trigger->SetQ2Min(1000.0);

      pythia6->register_trigger(trigger);
      
      se->registerSubsystem(pythia6);
    }


  DISKinematics *mcana = new DISKinematics(outputFile);
  se->registerSubsystem( mcana );


  //--------------
  // IO management
  //--------------

  /* Write DST output */
  if ( writedst )
    {
      Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT",outputFile);
      se->registerOutputManager(out);
    }

  /* Write HepMC ASCII output */
  else if ( writeascii )
    {
      Fun4AllHepMCOutputManager *asciiout = new Fun4AllHepMCOutputManager("HEPMCOUT",outputFile);
      se->registerOutputManager(asciiout);
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
