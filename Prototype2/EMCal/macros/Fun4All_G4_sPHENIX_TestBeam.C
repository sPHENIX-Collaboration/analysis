
int Fun4All_G4_sPHENIX_TestBeam(
		       const int nEvents = 100,
		           const char * inputFile ="./beam_00002256-0000.root"
		       )
{


  TString s_outputFile = inputFile;
  s_outputFile += "_Ana.root";
  const char * outputFile = s_outputFile.Data();

  
  //======================
  // What to run
  //======================

 
  //===============
  // Input options
  //===============

  // Either:
  // read previously generated g4-hits files, in this case it opens a DST and skips
  // the simulations step completely. The G4Setup macro is only loaded to get information
  // about the number of layers used for the cell reco code
  const bool readhits = true;
  // Or:
  // read files in HepMC format (typically output from event generators like hijing or pythia)
  const bool readhepmc = false; // read HepMC files
  // Or:
  // Use particle generator
  const bool runpythia = false;

  //---------------
  // Load libraries
  //---------------

  gSystem->Load("libfun4all.so");
   gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");
  gSystem->Load("libcemc.so");
  gSystem->Load("libg4eval.so");
   gSystem->Load("libPrototype2.so");

  bool do_cemc = true;
  bool do_cemc_cell = true;
  bool do_cemc_twr = true;
  bool do_cemc_cluster = true;
  bool do_cemc_eval = true;



  //Option to convert DST to human command readable TTree for quick poke around the outputs
  bool do_DSTReader = false;

 
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
  emcal_ana->set_flag(EMCalCalib_TestBeam::kProcessTower);

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
  if (nEvents == 0 && !readhits && !readhepmc)
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
