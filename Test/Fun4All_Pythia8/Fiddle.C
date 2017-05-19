int Fiddle()
{
	/// load some libraries.
	/// --------------------
	/// (1) Fun4All is sPHENIX's analysis framework.
	/// (2) HepMC is a plaintext output format for Monte Carlos.
	///     ...PHHepMC is presumably an sPHENIX wrapper :P
	/// (3) PHPythia8 is an sPHENIX wrapper on Pythia8::Pythia.
	
	gSystem->Load("libfun4all.so");
	gSystem->Load("libphhepmc.so");
	gSystem->Load("libPHPythia8.so");
	
	/// start the F4A server.
	/// ---------------------
	
	Fun4AllServer *se = Fun4AllServer::instance();
	
	/// register the analysis task.
	/// ---------------------------
	/// in this case, out task is PHPythia8.
	/// (it inherits from SubsysReco)
	/// the PHPythia8 config file is exactly the same
	/// as a normal Pythia8::Pythia config file
	/// ...see PHPythia8::Init(),
	/// ...and PHPythia8::read_config().
	/// a default config lives at coresoftware/generators/PHPythia8/phpythia8.cfg
	
	PHPythia8 *pythia8 = new PHPythia8();
	pythia8->set_config_file("phpythia8.cfg"); 
	se->registerSubsystem(pythia8);
	
	/// register the input/output.
	/// --------------------------
	/// we are generating our own events,
	/// so there is no real input .... JADE.
	
	Fun4AllInputManager *in = new Fun4AllDummyInputManager("JADE");
	se->registerInputManager(in);
	
	/// HepMC is *the best* output format
	/// ...i am not joking i love plaintext.
	
	Fun4AllHepMCOutputManager *hepmc = new Fun4AllHepMCOutputManager("HEPMCOUT", "Fiddle_OUT.dat");
	se->registerOutputManager(hepmc);
	
	/// run the analysis task.
	/// ----------------------
	
	int nEvents = 10;
	se->run(nEvents);
	
	/// clean up.
	/// ---------
	
	se->End();
	delete se;
	gSystem->Exit(0);
}

/// abbreviate file with just the code:
/// $ cat Fiddle.C | sed '/\/\/\//d' > Fiddle_CODE.C
