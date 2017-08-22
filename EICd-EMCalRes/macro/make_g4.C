int make_g4(
	const char *filename_g4,
	const char *flavor,
	Float_t pseudorapidity,
	Float_t momentum)
{
	///--------------
	/// INTRODUCTION
	///--------------
	
	/// this macro is designed to...
	/// 	
	/// 	simulate electron production in the EIC detector
	/// 	simulate electron interaction with the EIC detector
	
	/// it uses only one specified flavor/energy/pseudorapidity tuple.
	
	/// it runs 1,000 single-particle simulations.
	/// it produces a GEANT4 hits DST output file.
	
	///-----------------------
	/// LOAD LIBRARIES/MACROS
	///-----------------------
	
	gSystem->Load("libfun4all.so");
	gSystem->Load("libg4testbench.so");
	
	gSystem->Load("libg4detectors.so"); //this library is needed 'cause i am angry
	
	gROOT->SetMacroPath("/sphenix/user/jordan/sphenix-git/macros/macros/g4simulations");
	gROOT->LoadMacro("G4Setup_EICDetector.C");
	
	///---------------------
	/// SET UP INPUT/OUTPUT
	///---------------------
	
	/// we are generating all our own events, so no real input manager.
	/// still need DummyInputManager to drive the event loop, though.
	
	Fun4AllServer *se = Fun4AllServer::instance();
	
	Fun4AllDummyInputManager *in = new Fun4AllDummyInputManager("JADE");
	se->registerInputManager(in);
	
	Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", filename_g4);
	se->registerOutputManager(out);
	
	///---------------------------
	/// SET UP PARTICLE GENERATOR
	///---------------------------
	
	/// chris generates particles with pseudorapidities within 0.05 of nominal.
	/// my spread is broader 'cause my pseudorapidity coverage is broader.
	
	/// also, chris samples his collision vertex z-coordinates from a uniform distribution.
	/// i chose a gaussian 'cause i felt that it was more accurate.
	/// ALSO, i use a 27cm standard deviation, not 10cm.
	/// as i understand it, eRHIC vertex sigma will be 27cm.
	
	/// like chris, i randomly sample uniformly from full azimuth.
	/// like chris, i use exact nominal momentum values.
	
	PHG4SimpleEventGenerator *gen = new PHG4SimpleEventGenerator();
	gen->add_particles(flavor, 1);
	gen->set_p_range(momentum, momentum);
	gen->set_eta_range(pseudorapidity-.2, pseudorapidity+.2);
	gen->set_phi_range(-1.*TMath::Pi(), 1.*TMath::Pi());
	gen->set_vertex_distribution_function(
		PHG4SimpleEventGenerator::Gaus,
		PHG4SimpleEventGenerator::Gaus,
		PHG4SimpleEventGenerator::Gaus);
	gen->set_vertex_distribution_width(0., 0., 27.);
	se->registerSubsystem(gen);
	
	///----------------------------
	/// SET UP DETECTOR SIMULATION
	///----------------------------
	
	/// i am *not* confident that G4Setup_EICDetector.C correctly represents ePHENIX.
	
	/// most of the boolean arguments in G4Setup() are default.
	/// do_FEMC is false by default, however.. here it is true.
	
	G4Init();
	G4Setup(
		1,
		"/phenix/upgrades/decadal/fieldmaps/sPHENIX.2d.root",
		TPythia6Decayer::kAll,
		true, //SVTX
		true, //CEMC
		true, //inner HCAL
		true, //magnet
		true, //outer HCAL
		true, //beam pipe
		true, //FGEM
		true, //EGEM
		true); //FEMC
	
	///---------------------------
	/// RUN EVENT LOOP / CLEAN UP
	///---------------------------
	
	//se->run(1000);
	se->run(200);
	//se->run(10);
	se->End();
	return 0;
}
