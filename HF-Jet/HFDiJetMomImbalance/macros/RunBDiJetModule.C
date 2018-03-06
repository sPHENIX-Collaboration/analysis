
void RunBDiJetModule(
    const int nEvents = 0,
    // const char* input = "/phenix/plhf/dcm07e/sPHENIX/bjetsims/dst/pythia8_bjets_10_0.root",
    // const char* input = "/gpfs/mnt/gpfs04/sphenix/user/shlim/01.Tracking/jobdir_SVReco_all_6th_ana45/DST_PYTHIA_run001874.root",
    const char* input = "/gpfs/mnt/gpfs02/phenix/hhj/hhj3/dcm07e/sPHENIX/macros/macros/g4simulations/G4sPHENIX.root",
    const char* output = "test/test.root"
		) 
{

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");
  gSystem->Load("libg4calo.so");
  gSystem->Load("libg4eval.so");

  gSystem->Load("libBDiJetModule.so");

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0); 

  //--------
  // Modules
  //--------

  BDiJetModule *tm = new BDiJetModule( "BDiJetModule", output );
  se->registerSubsystem( tm );

  //------
  // Input
  //------

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");

	TString tstr_input(input);
	if (tstr_input.EndsWith(".root"))
		in->AddFile( input );
	else 
		in->AddListFile( input );

	se->registerInputManager(in);

  //----
  // Run
  //----

	se->run( nEvents );

  //-----
  // Exit
  //-----

	se->End();
  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);

}
