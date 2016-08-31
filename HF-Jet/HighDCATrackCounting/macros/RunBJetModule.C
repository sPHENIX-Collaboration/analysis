void RunBJetModule(
		const char* input = "/gpfs/mnt/gpfs02/sphenix/user/yuhw/workspace/BJetTagging/condor/output/0/sim_30GeV_ljet000.root",
		const char* output = "HFtag_ljet.root"//HFtag_ljet.root, HFtag_cjet.root, HFtag_bjet.root
		) {

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4eval.so");

  gSystem->Load("libBJetModule.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0); 

  BJetModule *tm;

  tm = new BJetModule( output );

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
