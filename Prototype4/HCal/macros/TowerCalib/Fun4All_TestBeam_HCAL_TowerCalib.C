#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDummyInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllNoSyncDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <proto4showercalib/Proto4ShowerCalib.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libPrototype4.so)
R__LOAD_LIBRARY(/direct/phenix+u/xusun/WorkSpace/sPHENIX/install/lib/libProto4HCalTowerCalib.so)
#endif

int
Fun4All_TestBeam_HCAL_TowerCalib(const int nEvents = 50000, const int colID = 0, const string det = "HCALOUT")
// Fun4All_TestBeam_HCAL_TowerCalib(const int nEvents = 1000, const int runID = 0)
{

  //---------------
  // Load libraries
  //---------------
  gSystem->Load("libPrototype4.so");
  gSystem->Load("libfun4all.so");                                                                        
  // gSystem->Load("libg4detectors.so");
  // gSystem->Load("libphhepmc.so");
  // gSystem->Load("libg4testbench.so");
  // gSystem->Load("libg4hough.so");
  // gSystem->Load("libg4eval.so");

  //---------------
  // Fun4All server
  //---------------

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  //-------------- 
  // IO management
  //--------------

  bool _is_sim = false;
  // Hits file
  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
  std::string inputlist;
  if(_is_sim) inputlist = Form("../list/Cosmic_SIM_%s_col_%d.list",det.c_str(),colID);
  if(!_is_sim) inputlist = Form("../list/Cosmic_RAW_%s_col_%d.list",det.c_str(),colID);
  hitsin->AddListFile(inputlist); // you can also choose this and give a list of DST file names in the file.
  se->registerInputManager(hitsin);

  //load your analysis module's lib
  gSystem->Load("/direct/phenix+u/xusun/WorkSpace/sPHENIX/install/lib/libProto4HCalTowerCalib.so");

  //load your analysis module.
  std::string outputfile;
  if(_is_sim) outputfile = Form("/sphenix/user/xusun/TestBeam/TowerCalib/Proto4TowerInfoSIM_%s_%d.root",det.c_str(),colID);
  if(!_is_sim) outputfile = Form("/sphenix/user/xusun/TestBeam/TowerCalib/Proto4TowerInfoRAW_%s_%d.root",det.c_str(),colID);

  Proto4TowerCalib* hcal_ana = new Proto4TowerCalib(outputfile.c_str());
  hcal_ana->is_sim(_is_sim);
  se->registerSubsystem(hcal_ana);

  se->run(nEvents);
  se->End();

  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
}
