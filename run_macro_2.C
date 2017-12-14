int run_macro_2(
                std::string infile = "G4sPHENIX_2.root",
                std::string outfile = "test.root"
                )
{

  gSystem->Load("libfun4all.so");
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");
  gSystem->Load("libcemc.so");
  gSystem->Load("libg4eval.so");
  gSystem->Load("libcalotrigger.so");
  gSystem->Load("libjetbackground.so");

  gSystem->Load("libTreeMaker.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity( 0 );
  // just if we set some flags somewhere in this macro
  recoConsts *rc = recoConsts::instance();

  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
  //hitsin->fileopen("");
  //hitsin->fileopen("$SPHENIX/blank/macros/macros/g4simulations/output/outputDijetTrig_35.root");
  hitsin->fileopen( infile );
  se->registerInputManager(hitsin);

  TreeMaker *tt = new TreeMaker( outfile );
  se->registerSubsystem( tt );

  se->run( 1 );

  se->End();
  std::cout << "All done" << std::endl;
  delete se;

  gSystem->Exit(0);
}
