#include <string>

using namespace std;

void Fun4All_TestBeam_ShowerCalib(  //
    const int nEvents = 10000000, const char *inputFile =
                                      //                                       "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/ShowerCalib/dst.lst"//
                                  //                                   "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/ShowerCalib_tilted/dst.lst"  //
                                  //        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/Scan1Block36/dst.lst"
                                  //"/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/Scan1Block27/dst.lst"
          "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/5thPositionScan/dst.lst"
//        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/4thPositionScan/dst.lst"
//        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/Scan2018b1Tower36/dst.lst"
    //        "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/Scan4Block45/dst.lst"
    //            "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/Scan64.28V/dst.lst"
    //                "/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/Scan2Block34/dst.lst"
    )
{
  TString s_outputFile = inputFile;
  s_outputFile += "_Ana.root";
  const char *outputFile = s_outputFile.Data();

  //---------------
  // Load libraries
  //---------------
  gSystem->Load("libProto4ShowCalib.so");

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

  Proto4ShowerCalib *emcal_ana = new Proto4ShowerCalib(
      string(inputFile) + string("_EMCalCalib.root"));

  emcal_ana->Verbosity(1);
  //  emcal_ana->LoadRecalibMap("/gpfs/mnt/gpfs04/sphenix/user/jinhuang/Prototype_2018/ShowerCalibFit_CablibConst.dat");
//  emcal_ana->LoadRecalibMap("/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/4thPositionScan/4thPositionScan_ShowerCalibFit_CablibConst.dat");
  emcal_ana->LoadRecalibMap("/phenix/u/jinhuang/links/sPHENIX_work/Prototype_2018/Scan2018b1Tower36/ManualFix_4thPositionScan_ShowerCalibFit_CablibConst.dat");
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
