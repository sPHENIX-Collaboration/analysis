
/*!
 * \file Fun4All_TestBeam_ExampleAnalysisModule.C
 * \brief // this macro is part of analysis tutorial: https://wiki.bnl.gov/sPHENIX/index.php/2017_calorimeter_beam_test/Tutorial#Level_2:_formal_analysis_via_analysis_module
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

int
Fun4All_TestBeam_HCAL_TowerCalib(const int nEvents = 50000, const int colID = 0, const string det = "HCALIN")
// Fun4All_TestBeam_HCAL_TowerCalib(const int nEvents = 1000, const int runID = 0)
{

  //---------------
  // Load libraries
  //---------------
  gSystem->Load("libPrototype4.so");
  gSystem->Load("libProto4_HCalTowerCalib.so");
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

  bool _is_sim = true;
  // Hits file
  Fun4AllInputManager *hitsin = new Fun4AllDstInputManager("DSTin");
  std::string inputlist;
  if(_is_sim) inputlist = Form("list/Cosmic_SIM_%s_col_%d.list",det.c_str(),colID);
  if(!_is_sim) inputlist = Form("list/Cosmic_RAW_%s_col_%d.list",det.c_str(),colID);
  hitsin->AddListFile(inputlist); // you can also choose this and give a list of DST file names in the file.
  se->registerInputManager(hitsin);

  //load your analysis module's lib
  gSystem->Load("libProto4_HCalTowerCalib.so");

  //load your analysis module.
  std::string outputfile;
  if(_is_sim) outputfile = Form("/sphenix/user/xusun/software/data/cosmic/TowerCalib/Proto4TowerInfoSIM_%s_%d.root",det.c_str(),colID);
  if(!_is_sim) outputfile = Form("/sphenix/user/xusun/software/data/cosmic/TowerCalib/Proto4TowerInfoRAW_%s_%d.root",det.c_str(),colID);

  Proto4TowerCalib* hcal_ana = new Proto4TowerCalib(outputfile.c_str());
  hcal_ana->is_sim(_is_sim);
  se->registerSubsystem(hcal_ana);

  se->run(nEvents);
  se->End();

  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
}
