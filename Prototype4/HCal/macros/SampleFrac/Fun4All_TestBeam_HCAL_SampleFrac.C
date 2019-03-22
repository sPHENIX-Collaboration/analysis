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
#include <proto4samplefrac/Proto4SampleFrac.h>

R__LOAD_LIBRARY(libPrototype4.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(/direct/phenix+u/xusun/WorkSpace/sPHENIX/install/lib/libProto4HCalSampleFrac.so)
#endif

int
// Fun4All_TestBeam_HCAL_SampleFrac(const int nEvents = 1000, const int posID = 0, const int addID = 0, const string det = "HCALOUT")
// Fun4All_TestBeam_HCAL_SampleFrac(const int nEvents = 1000, const int runID = 0, const string det = "HCALIN")
Fun4All_TestBeam_HCAL_SampleFrac(const int nEvents = 100000, const string det = "HCALOUT")
{

  //---------------
  // Load libraries
  //---------------
  gSystem->Load("libPrototype4.so");
  gSystem->Load("libfun4all.so");                                                                        

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
  // std::string inputFile = Form("/sphenix/user/xusun/software/macros/macros/prototype4/data/Simulation/Cosmic_orig/Cosmic_%s_%d%d.root",det.c_str(),posID,addID);
  // std::string inputFile = Form("/sphenix/user/xusun/software/macros/macros/prototype4/data/Simulation/Cosmic_Scint/Cosmic_%s_%d%d.root",det.c_str(),posID,addID);
  // std::string inputFile = Form("/sphenix/user/xusun/software/macros/macros/prototype4/data/Simulation/BeamTest_SF_orig/BeamTest_SF_%d.root",runID);
  // std::string inputFile = Form("/sphenix/user/xusun/software/macros/macros/prototype4/data/Simulation/BeamTest_SF_Scint/BeamTest_SF_%d.root",runID);
  // hitsin->fileopen(inputFile.c_str());
  std::string inputlist = "../list/HCAL_Prototype4_SIM_SampleFrac.list";
  hitsin->AddListFile(inputlist); // you can also choose this and give a list of DST file names in the file.
  se->registerInputManager(hitsin);

  //load your analysis module's lib
  gSystem->Load("/direct/phenix+u/xusun/WorkSpace/sPHENIX/install/lib/libProto4HCalSampleFrac.so");

  //load your analysis module.
  // std::string outputFile = Form("/sphenix/user/xusun/software/data/beam/SampleFrac/Proto4SampleFrac_%s_%d%d.root",det.c_str(),posID,addID);
  // std::string outputFile = Form("/sphenix/user/xusun/software/data/beam/SampleFrac/Proto4SampleFrac_%s_%d.root",det.c_str(),runID);
  std::string outputFile = Form("/sphenix/user/xusun/TestBeam/SampleFrac/Proto4SampleFrac_%s.root",det.c_str());
  Proto4SampleFrac* hcal_ana = new Proto4SampleFrac(det.c_str(),outputFile.c_str());
  se->registerSubsystem(hcal_ana);

  se->run(nEvents);
  se->End();

  std::cout << "All done" << std::endl;
  delete se;

  return 0;
}
