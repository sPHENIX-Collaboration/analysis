void Fun4All_Ana(int nEvents = 1000,
	char *input_file = "/gpfs02/phenix/prod/sPHENIX/sunyrefnewinnerhcal/fieldmap/G4Hits_sPHENIX_mu-_eta0.30_40GeV.root",
 	//char *input_file ="/sphenix/sim/sim01/cd1_review/single_particle/fieldmap/G4Hits_sPHENIX_e-_eta0.30_16GeV-0003.root",
        const char *outFile = "outfile.root")
{
  gSystem->Load("libfun4all");
  gSystem->Load("libphg4hit.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4picoDst.so"); //??
  gSystem->Load("libg4decayer.so");
  gSystem->Load("libg4eval.so");   
  gSystem->Load("libg4hough.so"); 
  gSystem->Load("libPHAna.so");//??

  ///////////////////////////////////////////
  //
  //- recoConsts setup
  //
  ///////////////////////////////////////////
  recoConsts *rc = recoConsts::instance();

  ///////////////////////////////////////////
  //
  //- Make the Server
  //
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);
  recoConsts *rc = recoConsts::instance();

  PHAna *ana = new PHAna();
  //ana->set_flag( PHAna::TRUTH, true);
  //ana->set_flag( PHAna::HIST, true);
  //ana->set_flag( PHAna::SF, true);
  ana->set_flag( PHAna::ALL, true);
  ana->set_filename( outFile );
  se->registerSubsystem( ana );

  ///////////////////////////////////////////
  //
  //- Analyze the Data.
  //
  //////////////////////////////////////////

  Fun4AllInputManager *hcal_dst = new Fun4AllDstInputManager( "HCAL", "DST", "TOP");
  se->registerInputManager( hcal_dst );
  se->fileopen( hcal_dst->Name(), input_file );
  //you can put a list of files as well 
  //hcal_dst->AddListFile( input_file );   

  se->run(nEvents);
  se->End();
  delete se;
}
