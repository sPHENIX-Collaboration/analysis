/// HCalInner() thinks that there guys are declared
/// in the global scope, so i need to put them here
/// or the function will freak out on me.
/// ...hopefully, somebody fixes this eventually :/

bool overlapcheck = 0;
double no_overlapp = 0;

int Fiddle()
{
	/// load some libraries.
	/// --------------------
	/// (1) Fun4All is sPHENIX's analysis framework.
	/// (2) PHHepMC is an sPHENIX wrapper for the HepMC data format.
	/// (3) you would think that libcemc.so contain code for the CEMCal,
	///     but it turns out to be necessary for the HCal as well.
	///     ...also it should be prefixed with g4,
	///     ...'cause it is for GEANT4 stuff. i think.
	/// (4) libg4vertex.so has code for global vertex reconstruction.
	/// (5,6) libg4jets.so and libg4eval.so are needed to use
	///     standard jet reconstruction/evaluation techniques.
	///     i am not 100% sure what they do exactly.
	
	gSystem->Load("libfun4all.so");
	gSystem->Load("libphhepmc.so");
	gSystem->Load("libcemc.so");
	gSystem->Load("libg4vertex.so");
	gSystem->Load("libg4jets.so");
	gSystem->Load("libg4eval.so");
	
	/// connect the input.
	/// ------------------
	
	Fun4AllServer *se = Fun4AllServer::instance();
	
	Fun4AllInputManager *in = new Fun4AllHepMCInputManager("HEPMCIN");
	se->registerInputManager(in);
	se->fileopen(in->Name().c_str(), "hepmc.dat");
	
	HepMCNodeReader *hr = new HepMCNodeReader();
	se->registerSubsystem(hr);
	
	/// set up the detector.
	/// --------------------
	/// for this example, i try loading as little as possible.
	/// turns out, only a vertexer and a hadronic calorimeter
	/// are needed for jets. i choose SVTX and inner HCal,
	/// but you could go BBC or outer HCal if you wanted.
	/// obviously, more detectors would be fine.
	
	/// from what i understand, PHG4Reco is sPHENIX's cover of GEANT4.
	
	PHG4Reco *g4Reco = new PHG4Reco();
	
	/// as of my writing, the macros
	/// 
	/// 	G4_Svtx_maps_ladders+intt_ladders+tpc_KalmanPatRec.C
	/// and
	/// 	G4_HcalIn_ref.C
	/// 
	/// define the SVTX and inner HCal.
	/// the typical nomenclature, however, would be
	/// G4_Svtx.C and G4_HcalIn.C, so that
	/// is how i copied them into this directory.
	
	gROOT->LoadMacro("G4_Svtx.C");
	gROOT->LoadMacro("G4_HcalIn.C");
	
	int absorberactive = 1; //dunno.
	
	SvtxInit();
	Svtx(
		g4Reco,
		0, //"radius"... just for error checking. i like to live dangerously.
		absorberactive);
	
	HCalInnerInit();
	HCalInner(
		g4Reco,
		0, //"radius"
		0, //"crossings"... not actually used anywhere in the function body :P
		absorberactive);
	
	/// not 100% sure why PHG4TruthSubsystem is needed,
	/// but stuff fails if i try to leave it out.
	/// i am guessing that it officially logs
	/// all GEANT4 truth hit info.
	
	PHG4TruthSubsystem *truthsys = new PHG4TruthSubsystem();
	g4Reco->registerSubsystem(truthsys);
	
	se->registerSubsystem(g4Reco);
	
	/// ****IMPORORTANT NOTE!!****
	/// standard G4Init() and G4Setup() functions do exist,
	/// but it annoys me that no G4Reco() or G4Eval() exist
	/// to complete them. i have to explicitly impliment
	/// G4Reco() and G4Eval(), so i figured that
	/// i might as well expand G4Init() and G4Setup() also.
	/// i *highly* recommend that you use standard setup functions,
	/// especially if the developers fix stuff in the future.
	
	/// set up detector like a responsible citizen.
	/// -------------------------------------------
	///
	/// gROOT->LoadMacro("G4Setup_sPHENIX.C");
	///
	/// G4Init();
	/// G4Setup();
	///
	/// ...and that is it. right now the magnetic field defaults
	/// in G4Setup() might not be the best, but the developers
	/// should fix that at some point.
	
	/// reconstruct the jets.
	/// ---------------------
	/// truth jets have pretty light requirements,
	/// but the lowest-level reconstructed jets
	/// need calorimeter towers and vertices.
	
	Svtx_Cells(); //turn GEANT's (x,y,z) hits into hardware components
	Svtx_Reco();  //...and *then* do vertex reconstruction.
	HCALInner_Cells();
	HCALInner_Towers();
	
	/// this guy applies smearing to all vertices, i think.
	/// the jet analysis tasks will complain without it.
	
	GlobalVertexReco *vertexreco = new GlobalVertexReco();
	se->registerSubsystem(vertexreco);
	
	/// the standard jet analysis macro is G4_Jet.C,
	/// and it defines a reconstruction function JetReco()
	/// JetReco() configures a crapton of different
	/// jet reconstruction tasks, for a ton of different
	/// R-values. i do not need most of that functionality,
	/// so to cut down on analysis time i will not use JetReco().
	/// ...i generally recommend you use standard tools,
	/// but in this case i think it would be fine to forgo JetReco().
	
	JetReco *truthjetreco = new JetReco();
	truthjetreco->add_input(new TruthJetInput(Jet::PARTICLE));
	truthjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.3), "AntiKt_Truth_r03");
	truthjetreco->set_algo_node("ANTIKT");
	truthjetreco->set_input_node("TRUTH");
	se->registerSubsystem(truthjetreco);
	
	JetReco *towerjetreco = new JetReco();
	towerjetreco->add_input(new TowerJetInput(Jet::HCALIN_TOWER));
	towerjetreco->add_algo(new FastJetAlgo(Jet::ANTIKT, 0.3), "AntiKt_Tower_r03");
	towerjetreco->set_algo_node("ANTIKT");
	towerjetreco->set_input_node("TOWER");
	se->registerSubsystem(towerjetreco);
	
	/// G4_Jet.C also defines Jet_Eval(), but
	/// it is so simple and specialized that
	/// i cannot believe that it was meant as
	/// a standard analysis tool.
	
	JetEvaluator *jeteval = new JetEvaluator(
		"JETEVALUATOR",
		"AntiKt_Tower_r03",
		"AntiKt_Truth_r03",
		"g4jeteval.root");
	se->registerSubsystem(jeteval);
	
	/// run the analysis tasks.
	/// -----------------------
	
	int nEvents = 20;
	se->run(nEvents);
	
	/// clean up.
	/// ---------
	
	se->End();
	delete se;
	gSystem->Exit(0);
}
