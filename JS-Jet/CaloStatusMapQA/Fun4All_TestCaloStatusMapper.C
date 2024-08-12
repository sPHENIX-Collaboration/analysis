/// ---------------------------------------------------------------------------
/*! \file Fun4All_TestCaloStatusMapper.C
 *  Derek Anderson
 *  05.16.2024
 *
 *  A small Fun4All macro to test the 'CaloStatusMapper' module.
 */
// ----------------------------------------------------------------------------

#define FUN4ALL_TESTCALOSTATUSMAPPER_C

// c++ utilities
#include <string>
// ffa modules
#include <ffamodules/FlagHandler.h>
#include <ffamodules/CDBInterface.h>
// fun4all libraries
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
// phool utilities
#include <phool/recoConsts.h>
// qa utils
#include <qautils/QAHistManagerDef.h>
// module definitions
#include <calostatusmapper/CaloStatusMapper.h>

R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalostatusmapper.so)



// macro body -----------------------------------------------------------------

void Fun4All_TestCaloStatusMapper(
  const int runnumber = 43273,
  const int nEvents = 10,
  const int verbosity = 5,
  const std::string inFile = "/sphenix/lustre01/sphnxpro/commissioning/slurp/caloy2test/run_00042000_00042100/DST_CALO_run2pp_new_2024p001-00042072-0121.root",
  const std::string outFile = "test_qa.root"
) {

  // options ------------------------------------------------------------------

  // trigger cluster maker options
  CaloStatusMapperConfig cfg_mapper {
    .debug = true
  };

  // initialize f4a -----------------------------------------------------------

  Fun4AllServer* f4a = Fun4AllServer::instance();
  CDBInterface*  cdb = CDBInterface::instance();
  recoConsts*    rc  = recoConsts::instance();
  f4a -> Verbosity(verbosity);
  cdb -> Verbosity(verbosity);

  // grab lookup tables
  rc -> set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
  rc -> set_uint64Flag("TIMESTAMP", runnumber);

  // register inputs/outputs --------------------------------------------------

  Fun4AllDstInputManager* input = new Fun4AllDstInputManager("InputDstManager");
  input -> fileopen(inFile);
  f4a   -> registerInputManager(input);

  // register subsystem reco modules ------------------------------------------

  // map out status of calo towers
  CaloStatusMapper* mapper = new CaloStatusMapper("CaloStatusMapper");
  mapper -> SetConfig(cfg_mapper);
  mapper -> Verbosity(verbosity);
  f4a    -> registerSubsystem(mapper);

  // run modules and exit -----------------------------------------------------

  // run4all
  f4a -> run(nEvents);
  f4a -> End();

  // save qa output and exit
  QAHistManagerDef::saveQARootFile(outFile);  // TODO uncomment when ready
  delete f4a;

  // close and  exit
  gSystem -> Exit(0);
  return;

}

// end ------------------------------------------------------------------------
