
/*!
 * \file Fun4All_TestBeam_ExampleAnalysisModule.C
 * \brief // this macro is part of analysis tutorial: https://wiki.bnl.gov/sPHENIX/index.php/2017_calorimeter_beam_test/Tutorial#Level_2:_formal_analysis_via_analysis_module
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

int
// Fun4All_TestBeam_HCAL_SampleFrac(const int nEvents = 1000, const int posID = 0, const int addID = 0, const string det = "HCALOUT")
Fun4All_TestBeam_HCAL_SampleFrac(const int nEvents = 1000, const int runID = 0, const string det = "HCALIN")
{

  //---------------
  // Load libraries
  //---------------
  gSystem->Load("libPrototype4.so");
  gSystem->Load("libProto4_HCalSampleFrac.so");
  gSystem->Load("libfun4all.so");                                                                        
  gSystem->Load("libg4detectors.so");
  gSystem->Load("libphhepmc.so");
  gSystem->Load("libg4testbench.so");
  gSystem->Load("libg4hough.so");
  gSystem->Load("libg4eval.so");

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
  // std::string inputFile = Form("/sphenix/user/xusun/software/macros/macros/prototype4/data/Simulation/Cosmic_%s_%d%d.root",det.c_str(),posID,addID);
  std::string inputFile = Form("/sphenix/user/xusun/software/macros/macros/prototype4/data/Simulation/BeamTest_SF_%d.root",runID);
  hitsin->fileopen(inputFile.c_str());
//  hitsin->AddListFile(inputFile); // you can also choose this and give a list of DST file names in the file.
  se->registerInputManager(hitsin);

  //load your analysis module's lib
  gSystem->Load("libProto4_HCalSampleFrac.so");

  //load your analysis module.
  // std::string outputFile = Form("OutPut/SampleFrac/Proto4SampleFrac_%s_%d%d.root",det.c_str(),posID,addID);
  std::string outputFile = Form("OutPut/SampleFrac/Proto4SampleFrac_%s_%d.root",det.c_str(),runID);
  Proto4SampleFrac* hcal_ana = new Proto4SampleFrac(det.c_str(),outputFile.c_str());
  se->registerSubsystem(hcal_ana);

  se->run(nEvents);
  se->End();

  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
}
