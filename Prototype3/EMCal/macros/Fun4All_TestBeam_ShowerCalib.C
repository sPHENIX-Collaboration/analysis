int
Fun4All_TestBeam_ShowerCalib(const int nEvents = 10000000, const char * inputFile =
    "/sphenix/user/jinhuang/Prototype_2017/ShowerCalib/JointEnergyScan1_Neg.lst")
{
  TString s_outputFile = inputFile;
  s_outputFile += "_Ana.root";
  const char * outputFile = s_outputFile.Data();

  //---------------
  // Load libraries
  //---------------
  gSystem->Load("libPrototype3.so");

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  //-------------- 
  // IO management
  //--------------

  // Hits file
  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
//  hitsin->fileopen(inputFile);
  hitsin->AddListFile(inputFile);
  se->registerInputManager(hitsin);

  gSystem->Load("libProto3ShowCalib.so");
  Proto3ShowerCalib * emcal_ana = new Proto3ShowerCalib(
      string(inputFile) + string("_EMCalCalib.root"));

  emcal_ana->Verbosity(1);
  emcal_ana->LoadRecalibMap( "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2017/ShowerCalib/ShowerCalibFit_CablibConst.dat");
  se->registerSubsystem(emcal_ana);

  cout << "nEVENTS :" << nEvents << endl;

  //-----------------
  // Event processing
  //-----------------
  if (nEvents < 0)
    {
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
