
int Fun4All_G4_EICIR(
		     const int nEvents = 1,
		     const char * inputFile = "/gpfs02/phenix/prod/sPHENIX/preCDR/pro.1-beta.5/single_particle/spacal1d/fieldmap/G4Hits_sPHENIX_e-_eta0_16GeV.root",
		     const char * outputFile = "G4EICIR.root"
		     )
{
  //===============
  // Input options
  //===============

  // Use particle gun
  const bool pgun = true;

  //======================
  // What to run
  //======================

  bool do_pipe = false;

  bool do_magnet = true;

  // Extended IR
  bool do_ExtendedIR = true;

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");
  gSystem->Load("libg4eval.so");

  // establish the geometry and reconstruction setup
  gROOT->LoadMacro("G4Setup_EICIR.C");
  G4Init(do_magnet,do_pipe,do_ExtendedIR);

  int absorberactive = 0; // set to 1 to make all absorbers active volumes
  //  const string magfield = "1.5"; // if like float -> solenoidal field in T, if string use as fieldmap name (including path)
  const string magfield = "/phenix/upgrades/decadal/fieldmaps/sPHENIX.2d.root"; // if like float -> solenoidal field in T, if string use as fieldmap name (including path)
  const float magfield_rescale = 1.4/1.5; // scale the map to a 1.4 T field

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();
  //se->Verbosity(100); // uncomment for batch production running with minimal output messages
  se->Verbosity(Fun4AllServer::VERBOSITY_SOME); // uncomment for some info for interactive running
  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();
  // By default every random number generator uses
  // PHRandomSeed() which reads /dev/urandom to get its seed
  // if the RANDOMSEED flag is set its value is taken as seed
  // You can either set this to a random value using PHRandomSeed()
  // which will make all seeds identical (not sure what the point of
  // this would be:
  //  rc->set_IntFlag("RANDOMSEED",PHRandomSeed());
  // or set it to a fixed value so you can debug your code
  // rc->set_IntFlag("RANDOMSEED", 12345);

  //-----------------
  // Event generation
  //-----------------

  if (pgun)
    {
      /* angle of particle phi:
	 pz = p * cos(psi)
	 px = p * sin(psi) */
      double psi_mrad = 0;

      double ptot = 250*1;

      double vx = 0;
      double vy = 0;
      double vz = 0;

      double px = ptot * sin(psi_mrad / 1000.);
      double py = 0;
      double pz = ptot * cos(psi_mrad / 1000.);

      PHG4ParticleGun*gun = new PHG4ParticleGun();
      gun->set_name("proton");
      gun->set_vtx(vx,vy,vz);
      gun->set_mom(px,py,pz);
      se->registerSubsystem(gun);
    }
  else
    {
      cout << "WARNING: No events being generated!" << endl;
    }

  //---------------------
  // Detector description
  //---------------------

  G4Setup(absorberactive, magfield, TPythia6Decayer::kAll,do_magnet,do_pipe,do_ExtendedIR,magfield_rescale);



  //--------------
  // IO management
  //--------------

  // for single particle generators we just need something which drives
  // the event loop, the Dummy Input Mgr does just that
  Fun4AllInputManager *in = new Fun4AllDummyInputManager( "JADE");
  se->registerInputManager( in );

  //Convert DST to human command readable TTree for quick poke around the outputs
  gROOT->LoadMacro("G4_DSTReader_EICIR.C");

  G4DSTreader_EICIR( outputFile, //
		     /*int*/ absorberactive );

  Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", outputFile);
  //if (do_dst_compress) DstCompress(out);
  se->registerOutputManager(out);

  if (nEvents == 0 && !readhits && !readhepmc)
    {
      cout << "using 0 for number of events is a bad idea when using particle generators" << endl;
      cout << "it will run forever, so I just return without running anything" << endl;
      return;
    }

  if (nEvents < 0)
    {
      PHG4Reco *g4 = (PHG4Reco *) se->getSubsysReco("PHG4RECO");
      g4->ApplyCommand("/control/execute vis.mac");
      //g4->StartGui();
      se->run(1);

      se->End();
      std::cout << "All done" << std::endl;

      std::cout << "==== Useful display commands ==" << std::endl;
      cout << "draw axis: " << endl;
      cout << " G4Cmd(\"/vis/scene/add/axes 0 0 0 50 cm\")" << endl;
      cout << "zoom" << endl;
      cout << " G4Cmd(\"/vis/viewer/zoom 1\")" << endl;
      cout << "viewpoint:" << endl;
      cout << " G4Cmd(\"/vis/viewer/set/viewpointThetaPhi 0 0\")" << endl;
      cout << "panTo:" << endl;
      cout << " G4Cmd(\"/vis/viewer/panTo 0 0 cm\")" << endl;
      cout << "print to eps:" << endl;
      cout << " G4Cmd(\"/vis/ogl/printEPS\")" << endl;
      cout << "set background color:" << endl;
      cout << " G4Cmd(\"/vis/viewer/set/background white\")" << endl;
      std::cout << "===============================" << std::endl;
    }
  else
    {
      se->run(nEvents);

      se->End();
      std::cout << "All done" << std::endl;
      delete se;
      gSystem->Exit(0);
    }

}


void
G4Cmd(const char * cmd)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  PHG4Reco *g4 = (PHG4Reco *) se->getSubsysReco("PHG4RECO");
  g4->ApplyCommand(cmd);
}
