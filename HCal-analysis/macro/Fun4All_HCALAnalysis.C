void Fun4All_HCALAnalysis(int nEvents = 0, char *input_file = "G4sPHENIXCells_LC.root", const char *outFile = "outfile_LC.root")
{
  gSystem->Load("libfun4all");
  gSystem->Load("libphg4hit.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libg4picoDst.so");
  gSystem->Load("libg4decayer.so");
  gSystem->Load("libg4eval.so");   
  gSystem->Load("libg4hough.so"); 
  gSystem->Load("libHCALAnalysis.so");
 
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
  //PHMapManager::set_verbosity(MUTOO::SOME);
  
  recoConsts *rc = recoConsts::instance();

  HCALAnalysis *an = new HCALAnalysis(outFile);
  se->registerSubsystem( an );

  ///////////////////////////////////////////
  //
  //- Analyze the Data.
  //
  //////////////////////////////////////////

  Fun4AllInputManager *hcal_dst = new Fun4AllDstInputManager( "HCAL", "DST", "TOP");
  se->registerInputManager( hcal_dst );
  se->fileopen( hcal_dst->Name(), input_file );
  
  se->run(nEvents);
  se->End();
  delete se;
  cout << "Completed DST spin." << endl;

  //gROOT->ProcessLine(".! mv test_tree_out.root calib_test_tree.root");
}
