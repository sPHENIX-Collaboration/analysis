
/*!
 * \file Fun4All_TestBeam_ExampleAnalysisModule.C
 * \brief // this macro is part of analysis tutorial: https://wiki.bnl.gov/sPHENIX/index.php/2017_calorimeter_beam_test/Tutorial#Level_2:_formal_analysis_via_analysis_module
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

int Fun4All_TestBeam_HCAL_ShowerCalib(const int nEvents = 10000000, const string runID = "0422")
// int Fun4All_TestBeam_HCAL_ShowerCalib(const int nEvents = 10, const string runID = "0422")
{

  //---------------
  // Load libraries
  //---------------
  gSystem->Load("/direct/phenix+u/xusun/WorkSpace/sPHENIX/install/lib/libPrototype4.so");
  // gSystem->Load("libPrototype4.so");
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

  bool _is_sim = false;
  // bool _is_sim = true;
  // Hits file
  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
  std::string inputfile;
  if(_is_sim) inputfile = Form("/sphenix/user/xusun/Simulation/Production/BeamTest_24GeV_%s.root",runID.c_str());
  if(!_is_sim) inputfile= Form("/sphenix/data/data02/sphenix/t1044/production/production_HCal_0221/beam_0000%s-0000.root",runID.c_str());
  // if(!_is_sim) inputfile= Form("/sphenix/user/xusun/TestBeam/Production/beam_0000%s.root",runID.c_str());
  hitsin->fileopen(inputfile);
//  hitsin->AddListFile(inputFile); // you can also choose this and give a list of DST file names in the file.
  se->registerInputManager(hitsin);

  //load your analysis module.
  gSystem->Load("libProto4_HCalShowerCalib.so");
  std::string outputfile;
  if(_is_sim) outputfile = Form("/sphenix/user/xusun/Simulation/ShowerCalib/Proto4ShowerInfoSIM_%s.root",runID.c_str());
  if(!_is_sim) outputfile = Form("/sphenix/user/xusun/TestBeam/ShowerCalib/Proto4ShowerInfoRAW_%s.root",runID.c_str());

  // This one is an example defined in ../ExampleAnalysisModule/
  Proto4ShowerCalib* hcal_ana = new Proto4ShowerCalib(outputfile.c_str());
  hcal_ana->is_sim(_is_sim);
  hcal_ana->set_runID(runID);
  se->registerSubsystem(hcal_ana);

  se->run(nEvents);
  se->End();

  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
}
