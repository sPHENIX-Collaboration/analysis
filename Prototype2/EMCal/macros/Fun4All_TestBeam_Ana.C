
int Fun4All_TestBeam_Ana(
		       const int nEvents = 100,
		           const char * inputFile ="./beam_00002256-0000.root"
		       )
{
  TString s_outputFile = inputFile;
  s_outputFile += "_Ana.root";
  const char * outputFile = s_outputFile.Data();
 

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libfun4all.so");
   gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");
  gSystem->Load("libg4calo.so");
  gSystem->Load("libg4eval.so");
  gSystem->Load("libPrototype2.so");
 
  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(10); 


  //-------------- 
  // IO management
  //--------------
 
      // Hits file
      Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
      hitsin->fileopen(inputFile);
      se->registerInputManager(hitsin);
 
  
  gSystem->Load("libemcal_calib.so");
 

  EMCalCalib_TestBeam * emcal_ana = new EMCalCalib_TestBeam(
  string(inputFile) + string("_EMCalCalib.root"));

  emcal_ana->Verbosity(10);
  se->registerSubsystem(emcal_ana);
  
 
  cout<<"nEVENTS :"<<nEvents<<endl;


  //-----------------
  // Event processing
  //-----------------
  if (nEvents < 0)
    {
      return;
    }
  // if we run the particle generator and use 0 it'll run forever
  if (nEvents == 0)
    {
      cout << "using 0 for number of events is a bad idea when using particle generators" << endl;
      cout << "it will run forever, so I just return without running anything" << endl;
      return;
    }

  gSystem->ListLibraries();

  se->run(nEvents);

  //-----
  // Exit
  //-----
  gSystem->Exec("ps -o sid,ppid,pid,user,comm,vsize,rssize,time");

  se->End();

  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
}
