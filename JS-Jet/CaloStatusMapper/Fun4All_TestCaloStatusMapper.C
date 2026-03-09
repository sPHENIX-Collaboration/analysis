/// ===========================================================================
/*! \file Fun4All_TestCaloStatusMapper.C
 *  Derek Anderson
 *  05.16.2024
 *
 *  A small Fun4All macro to test the 'CaloStatusMapper' module.
 */
/// ===========================================================================

#define FUN4ALL_TESTCALOSTATUSMAPPER_C

// c++ utilities
#include <string>
#include <utility>
// ffa modules
#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
// fun4all libraries
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/SubsysReco.h>
// jet qa utilities
#include <jetqa/JetQADefs.h>
// phool utilities
#include <phool/recoConsts.h>
// qa utils
#include <qautils/QAHistManagerDef.h>
// module definitions
#include <calostatusmapper/CaloStatusMapper.h>
#include <calostatusmapper/CaloStatusMapperDefs.h>

R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libcalostatusmapper.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libjetqa.so)



// macro body =================================================================

void Fun4All_TestCaloStatusMapper(
  const int nEvents = 100,
  const std::string& inlist = "input/dst_calo_run2pp-00047289.list",
  const std::string& outfile = "test.root",
  const std::string& outfile_hist = "test_hists.root",
  const std::string& dbtag = "ProdA_2024",
  const int verbosity = 1
) {

  // options ------------------------------------------------------------------

  // trigger cluster maker options
  CaloStatusMapper::Config cfg_mapper {
    .debug       = true,
    .histTag     = "Test",
    .doTrgSelect = false,
    .trgToSelect = JetQADefs::GL1::MBDNSJet1
  };

  // for example, we'll only look at the EMCal and OHCal
  cfg_mapper.inNodeNames = {
    {"TOWERINFO_CALIB_CEMC",    CaloStatusMapperDefs::Calo::EMCal},
    {"TOWERINFO_CALIB_HCALOUT", CaloStatusMapperDefs::Calo::HCal}
  };

  // initialize f4a -----------------------------------------------------------

  // initialize F4A server
  Fun4AllServer* se = Fun4AllServer::instance();
  se -> Verbosity(verbosity);

  // grab 1st file from input list
  ifstream    files(inlist);
  std::string first("");
  std::getline(files, first);

  // grab run and segment no.s
  std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(first);
  int runnumber = runseg.first;

  Fun4AllServer* f4a = Fun4AllServer::instance();
  CDBInterface*  cdb = CDBInterface::instance();
  recoConsts*    rc  = recoConsts::instance();
  f4a -> Verbosity(verbosity);
  cdb -> Verbosity(verbosity);

  // grab lookup tables
  rc -> set_StringFlag("CDB_GLOBALTAG", dbtag);
  rc -> set_uint64Flag("TIMESTAMP", runnumber);

  // connect to conditions database
  CDBInterface::instance()->Verbosity(verbosity);

  // set up flag handler
  FlagHandler* flag = new FlagHandler();
  se -> registerSubsystem(flag);

  // register inputs/outputs --------------------------------------------------

  Fun4AllDstInputManager* input = new Fun4AllDstInputManager("InputDstManager");
  input -> AddListFile(inlist);
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
  QAHistManagerDef::saveQARootFile(outfile_hist);
  delete f4a;

  // close and  exit
  gSystem -> Exit(0);
  return;

}

// end ========================================================================
