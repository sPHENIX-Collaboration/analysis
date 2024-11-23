/// ===========================================================================
/*! \file    Fun4All_TestBeamBackgroundFilterAndQA.C
 *  \authors Hanpu Jiang, Derek Anderson
 *  \date    10.21.2024
 *
 *  A small F4A macro to test the 'BeamBackgroundFilterAndQA' module.
 */
/// ===========================================================================

#define FUN4ALL_TESTBEAMBACKGROUNDFILTERANDQA_C

// c++ utilities
#include <string>

// f4a libraries
#include <beambackgroundfilterandqa/BeamBackgroundFilterAndQA.h>
#include <beambackgroundfilterandqa/NullFilter.h>
#include <beambackgroundfilterandqa/StreakSidebandFilter.h>
#include <beambackgroundfilterandqa/TestPHFlags.h>
#include <ffamodules/CDBInterface.h>
#include <FROG.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>
#include <phool/recoConsts.h>
#include <qautils/QAHistManagerDef.h>

R__LOAD_LIBRARY(libbeambackgroundfilterandqa.so)
R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)



// macro body =================================================================

void Fun4All_TestBeamBackgroundFilterAndQA(
  const int runnumber = 47152,
  const int nEvents = 10,
  const int verbosity = 0,
  const std::string inFile = "DST_CALO_run2pp_ana437_2024p007-00047152-00160.root",
  const std::string outFile = "test_bbfaq.root"
) {

  // options ------------------------------------------------------------------

  // options for null (template) algorithm
  NullFilter::Config cfg_null {
    .debug = true,
    .verbosity = verbosity
  };

  // options for streak sideband algorithm
  StreakSidebandFilter::Config cfg_sideband {
    .debug = true,
    .minStreakTwrEne = 0.6,
    .maxAdjacentTwrEne = 0.06,
    .minNumTwrsInStreak = 5,
    .verbosity = verbosity
  };

  // options for entire modle
  BeamBackgroundFilterAndQA::Config cfg_filter {
    .debug = true,
    .doQA = true,
    .doEvtAbort = false,
    .sideband = cfg_sideband
  };

  // initialize f4a -----------------------------------------------------------

  FROG*          frog = new FROG();
  Fun4AllServer* f4a  = Fun4AllServer::instance();
  CDBInterface*  cdb  = CDBInterface::instance();
  recoConsts*    rc   = recoConsts::instance();
  frog -> Verbosity(verbosity);
  f4a  -> Verbosity(verbosity);
  cdb  -> Verbosity(verbosity);

  // grab lookup tables
  rc -> set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
  rc -> set_uint64Flag("TIMESTAMP", runnumber);

  // register inputs/outputs --------------------------------------------------

  Fun4AllDstInputManager* input = new Fun4AllDstInputManager("InputDstManager");
  input -> fileopen(inFile);
  f4a   -> registerInputManager(input);

  // register subsystem reco modules ------------------------------------------

  // filter beam background events & QA them
  BeamBackgroundFilterAndQA* filter = new BeamBackgroundFilterAndQA("BeamBackgroundFilterAndQA");
  filter -> SetConfig(cfg_filter);
  filter -> Verbosity(verbosity);
  f4a    -> registerSubsystem(filter);

  // confirm that background flags exist
  TestPHFlags* flags = new TestPHFlags("TestPHFlags");
  f4a -> registerSubsystem(flags);

  // run modules and exit -----------------------------------------------------

  // run4all
  f4a -> run(nEvents);
  f4a -> End();

  // save qa output and exit
  if (cfg_filter.doQA) {
    QAHistManagerDef::saveQARootFile(outFile);
  }
  delete f4a;

  // close and  exit
  gSystem -> Exit(0);
  return;

}

// end ========================================================================
