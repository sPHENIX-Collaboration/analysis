
/*!
 * \file Fun4All_TestBeam_ExampleAnalysisModule.C
 * \brief // this macro is part of analysis tutorial: https://wiki.bnl.gov/sPHENIX/index.php/2017_calorimeter_beam_test/Tutorial#Level_2:_formal_analysis_via_analysis_module
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

int
Fun4All_TestBeam_ExampleAnalysisModule(const int nEvents = 10000,
    const char * inputFile =
        // Let's take a look at run 668, which is -24 GeV/c secondary beam centered on EMCal tower 18.
        // More runs are produced in data production: https://wiki.bnl.gov/sPHENIX/index.php/2018_calorimeter_beam_test/Data_Production_and_Analysis#Production_output
        "/sphenix/data/data03/phnxreco/sphenix/t1044/production/production_0306/beam_00000668-0000.root")
{

  //---------------
  // Load libraries
  //---------------
  gSystem->Load("libPrototype4.so");

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
  gSystem->Load("libProto4_ExampleAnalysisModule.so");

  //load your analysis module.
  // This one is an example defined in ../ExampleAnalysisModule/
  ExampleAnalysisModule * emcal_ana = new ExampleAnalysisModule(
      "ExampleAnalysis.root");
  se->registerSubsystem(emcal_ana);

  se->run(nEvents);
  se->End();

  std::cout << "All done" << std::endl;
  delete se;
  gSystem->Exit(0);
}
