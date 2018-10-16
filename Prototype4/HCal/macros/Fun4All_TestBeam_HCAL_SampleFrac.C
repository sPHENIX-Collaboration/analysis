
/*!
 * \file Fun4All_TestBeam_ExampleAnalysisModule.C
 * \brief // this macro is part of analysis tutorial: https://wiki.bnl.gov/sPHENIX/index.php/2017_calorimeter_beam_test/Tutorial#Level_2:_formal_analysis_via_analysis_module
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

int
Fun4All_TestBeam_HCAL_SampleFrac(const int nEvents = 1000,
    const char * inputFile =
        // Let's take a look at run 668, which is -24 GeV/c secondary beam centered on EMCal tower 18.
        // More runs are produced in data production: https://wiki.bnl.gov/sPHENIX/index.php/2018_calorimeter_beam_test/Data_Production_and_Analysis#Production_output
        "/sphenix/user/xusun/software/macros/macros/prototype4/G4Prototype4New.root")
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
  hitsin->fileopen(inputFile);
//  hitsin->AddListFile(inputFile); // you can also choose this and give a list of DST file names in the file.
  se->registerInputManager(hitsin);

  //load your analysis module's lib
  gSystem->Load("libProto4_HCalSampleFrac.so");

  //load your analysis module.
  // This one is an example defined in ../ExampleAnalysisModule/
  Proto4SampleFrac* hcal_ana = new Proto4SampleFrac(
      "OutPut/Proto4SampleFrac.root");
  se->registerSubsystem(hcal_ana);

  se->run(nEvents);
  se->End();

  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
}
