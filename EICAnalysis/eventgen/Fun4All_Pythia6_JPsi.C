int Fun4All_Pythia6(
                    const int nEvents = 10,
                    //const char * pythiaConfigFile = "phpythia6_e20p250_jpsi_jpsi2ee.cfg",
                    //const char * outputFile = "phpythia6_e20p250_jpsi_jpsi2ee_1k_hepmc.root"
                    const char * pythiaConfigFile = "phpythia6_e20p250_ccbar_jpsi2ee.cfg",
                    const char * outputFile = "phpythia6_e20p250_ccbar_jpsi2ee_1k_hepmc.root"
                    )
{

  bool runpythia6 = true;

  bool writedst = true;
  bool writeascii = false;

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libfun4all.so");
  gSystem->Load("libphhepmc.so");

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
      //pythia6->save_ascii("testascii.txt");
      se->registerSubsystem(pythia6);

      PHPy6ParticleTrigger* particleTrig = new PHPy6ParticleTrigger("Particle Trigger");
      particleTrig->AddParticles("-11");

      particleTrig->AddParents("443"); // 443 = J/psi meson

      //      particleTrig->Verbosity(6);

      pythia6->register_trigger((PHPy6GenTrigger *)particleTrig);
    }

//  //--------------
//  // Use HepMC based trigger
//  //--------------
//
//  PHHepMCParticleSelectorDecayProductChain *hepmctrig = new PHHepMCParticleSelectorDecayProductChain();
//  hepmctrig->SetParticle(443);
//  hepmctrig->AddDaughter(11);
//  hepmctrig->AddDaughter(-11);
//  hepmctrig->AddDaughter(13);
//  hepmctrig->AddDaughter(-13);
//  se->registerSubsystem(hepmctrig);

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
