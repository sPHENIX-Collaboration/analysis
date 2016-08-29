void generate_hepmc_bjet(
		int flavor = -1,
		char *output = "",
		int nevents = 100
		)
{
	//gSystem->Load("libfun4all.so");	// framework + reco modules
	//gSystem->Load("libPHPythiaEventGen.so");
	//gSystem->Load("libPHPythiaJet.so");
	//gSystem->Load("libPHJetBackgroundSubtract.so");
	//gSystem->Load("libHFJetAnalyzer.so");
	//gSystem->Load("libPythiaDumper.so");
	//gSystem->Load("libPHDetectorResponse.so");
	//gSystem->Load("libPHPythia.so");
	//gSystem->Load("libPHPyTrigger.so");		// For PHPyTrigger derived classes
	//gSystem->Load("libPHPyParticleSelect.so");	// For PHPyParticleSelect derived classes
	//gSystem->Load("libsimreco.so");	// framework + reco modules

	gSystem->Load("libPHPythia8.so");
	gSystem->Load("libfun4all.so");
	gSystem->Load("libphhepmc.so");

	recoConsts *rc = recoConsts::instance();
	rc->set_IntFlag("RUNNUMBER",0);

	/////////////////////////////////////////////////////////////////
	//  Server...
	Fun4AllServer *se = Fun4AllServer::instance();

	/////////////////////////////////////////////////////////////////
	//  Reconstruction Modules...

	//SubsysReco *sync = new SyncSimreco();
	//se->registerSubsystem(sync);

	PHPy8JetTrigger *theTrigger = new PHPy8JetTrigger();
	//theTrigger->Verbosity(10);
	theTrigger->SetEtaHighLow(-1, 1);
	theTrigger->SetJetR(.4);
	theTrigger->SetMinJetPt(20);

	PHPythia8* pythia8 = new PHPythia8();
	// see coresoftware/generators/PHPythia8 for example config
	if (flavor == 0) pythia8->set_config_file("phpythia8_ljet.cfg"); 
	if (flavor == 1) pythia8->set_config_file("phpythia8_cjet.cfg"); 
	if (flavor == 2) pythia8->set_config_file("phpythia8_bjet.cfg"); 
	pythia8->register_trigger(theTrigger);
	se->registerSubsystem(pythia8);


	//PHPythia *phpythia = new PHPythia();
	//phpythia->ReadConfig(  phpythiaconfig );  
	//se->registerSubsystem(phpythia);

	//PHPyJetParticleAcceptor *inpythia = new PHPyJetParticleAcceptInPerfect(3.0);
	//PHJetTowerAcceptor *inupgrade = new PHJetTowerAcceptInDetector(2.0);

	//PHPyJetMaker *PythiaJets_Antikt04 = new PHPyJetMaker("anti-kt",0.4,"PythiaJets_Antikt04",inpythia,"PHPythia");
	//PythiaJets_Antikt02->SetPtCut(10.0);
	//se->registerSubsystem(PythiaJets_Antikt04);

	//PythiaDumper *hfjt = new PythiaDumper();
	//se->registerSubsystem( hfjt );
	/*
		 PHMakeTowers *pythiatowermaker = new PHMakeTowers("PythiaTowerMaker","PHPythia","","PythiaTowers");
	//pythiatowermaker->SetResolutionParameters(0,0,ecal_sqrte,0);
	//pythiatowermaker->SetResolutionParameters(1,hcal_const,hcal_sqrte,hcal_e);
	se->registerSubsystem(pythiatowermaker);

	PHTowerJetMaker *pythiatowerreco2 = new PHTowerJetMaker("anti-kt",0.2,"PythiaTowerJets_Antikt02",inupgrade,"PythiaTowers");
	//pythiatowerreco2->Verbosity(verbosity+2);
	pythiatowerreco2->SetPtCut(10.0);
	se->registerSubsystem(pythiatowerreco2);
	*/


	//** You can force the generated particles to use a vertex read from a file,
	//** in place of the default (z=0) value
	//** this is needed for instance when you want to have matching vertices between 
	//** different types of simulated files, prior to sending that to PISA
	// se->registerSubsystem( new PHPyVertexShift( "PHPyVertexShift", "./events.txt") );

	//** You can use dedicated triggers, derived from the PHPyTrigger base class
	// se->registerSubsystem( new PHPyJPsiMuonTrigger() );

	//** You can select only particular particles to write out
	//PHPyParticleSelect *pselect = new PHPyParticleSelect();
	//se->registerSubsystem( pselect );

	//** A dummy (null) input is needed for the Fun4All framework
	Fun4AllDummyInputManager *in1 = new Fun4AllDummyInputManager("DSTin1", "DST");
	se->registerInputManager(in1);


	// DST output manager
	Fun4AllDstOutputManager *dst_output_mgr  = new Fun4AllDstOutputManager("PHPYTHIA", "temp.root" );
	dst_output_mgr->AddNode("Sync");
	dst_output_mgr->AddNode("PHPythiaHeader");
	dst_output_mgr->AddNode("PHPythia");

	Fun4AllHepMCOutputManager *hepmc;
	hepmc = new Fun4AllHepMCOutputManager("HEPMCOUT", output );
	/*
		 if (flavor == 0) hepmc = new Fun4AllHepMCOutputManager("HEPMCOUT", "output/hepmc_ljet_10k.txt" );
		 if (flavor == 1) hepmc = new Fun4AllHepMCOutputManager("HEPMCOUT", "output/hepmc_cjet_10k.txt" );
		 if (flavor == 2) hepmc = new Fun4AllHepMCOutputManager("HEPMCOUT", "output/hepmc_bjet_10k.txt" );
		 */
	se->registerOutputManager( hepmc );

	//dst_output_mgr->AddNode("PythiaTowers");
	//dst_output_mgr->AddNode("PythiaJets_Antikt04");
	//dst_output_mgr->AddNode("PythiaTowerJets_Antikt02");

	//  se->registerOutputManager(dst_output_mgr);

	// OSCAR output manager
	// with following output manager, one can write the PHPythia output in an oscar formated output text file
	// PHPyOscarOutputManager *oscar_manager  = new PHPyOscarOutputManager( "OSCAR", oscar_outputname );
	// se->registerOutputManager(oscar_manager);

	// run over all events
	se->run( nevents );  
	se->End();
}

