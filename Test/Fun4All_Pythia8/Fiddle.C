int Fiddle()
{
	gSystem->Load("libfun4all.so");
	gSystem->Load("libphhepmc.so");
	Fun4AllServer *se = Fun4AllServer::instance();
	se->Verbosity(0);
	gSystem->Load("libPHPythia8.so");
	PHPythia8* pythia8 = new PHPythia8();
	pythia8->set_config_file("phpythia8.cfg"); 
	se->registerSubsystem(pythia8);
	HepMCNodeReader *hr = new HepMCNodeReader();
	se->registerSubsystem(hr);
	Fun4AllInputManager *in = new Fun4AllDummyInputManager( "JADE");
	se->registerInputManager( in );
	Fun4AllHepMCOutputManager *hepmc = new Fun4AllHepMCOutputManager("HEPMCOUT", "Fiddle_OUT.dat");
	se->registerOutputManager(hepmc);
	se->run(10);
	se->End();
	delete se;
	gSystem->Exit(0);
}
