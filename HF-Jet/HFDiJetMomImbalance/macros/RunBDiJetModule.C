void RunBDiJetModule(
		const char* input = "/phenix/hhj3/dcm07e/sPHENIX/macros/macros/g4simulations/G4sPHENIXCells.root",
		const char* output = "HFtag_bjet.root"
		) {


  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");
  gSystem->Load("libcemc.so");
  gSystem->Load("libg4eval.so");

  // gSystem->Load("libfun4all.so");
  // gSystem->Load("libg4eval.so");

  gSystem->Load("libBDiJetModule.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0); 

  BDiJetModule *tm;

  tm = new BDiJetModule( output );

  se->registerSubsystem( tm );

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTin");

	TString tstr_input(input);
	if (tstr_input.EndsWith(".root"))
		in->AddFile( input );
	else 
		in->AddListFile( input );

	se->registerInputManager(in);

	se->run( 0 );

	se->End();

}
