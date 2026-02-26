// c++ includes --
#include <string>
#include <iostream>

// root includes --
#include <TSystem.h>
#include <TROOT.h>
#include <TF1.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllUtils.h>

#include <caloreco/CaloTowerStatus.h>

#include <eventplaneinfo/EventPlaneRecov2.h>

#include <phool/recoConsts.h>

#include <sepdvalidation/sEPD_DataMC_Validation.h>

R__LOAD_LIBRARY(libsEPDValidation.so)

void Fun4All_sEPD_DataMC(const std::string& fname_global,
                         const std::string& fname_jets,
                         const std::string& input_QVecCalib,
                         const std::string& input_sEPD_BadTowers,
                         const std::string& output = "test.root",
                         int nEvents = 100,
                         const std::string& dbtag = "newcdbtag")
{
  std::cout << "########################" << std::endl;
  std::cout << "Run Parameters" << std::endl;
  std::cout << "input global: " << fname_global << std::endl;
  std::cout << "input jets: " << fname_jets << std::endl;
  std::cout << "input QVecCalib: " << input_QVecCalib << std::endl;
  std::cout << "input Bad Towers: " << input_sEPD_BadTowers << std::endl;
  std::cout << "output: " << output << std::endl;
  std::cout << "nEvents: " << nEvents << std::endl;
  std::cout << "dbtag: " << dbtag << std::endl;
  std::cout << "########################" << std::endl;

  /* Verbosity Options
  Quiet mode. Only output critical messages. Intended for batch production mode.
  VERBOSITY_QUIET = 0,

  Output some useful messages during manual command line running
  VERBOSITY_SOME = 1,

  Output more messages
  VERBOSITY_MORE = 2,

  Output even more messages
  VERBOSITY_EVEN_MORE = 3,

  Output a lot of messages
  VERBOSITY_A_LOT = 4,
   */

  Fun4AllServer *se = Fun4AllServer::instance();

  recoConsts *rc = recoConsts::instance();

  std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(fname_global);
  unsigned int runnumber = static_cast<unsigned int>(runseg.first);

  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(Fun4AllBase::VERBOSITY_SOME);

  FlagHandler* flag = new FlagHandler();
  se->registerSubsystem(flag);

  // sEPD Bad Tower Status
  CaloTowerStatus* statusSEPD = new CaloTowerStatus("SEPDSTATUS");
  statusSEPD->set_detector_type(CaloTowerDefs::SEPD);
  statusSEPD->set_directURL_hotMap(input_sEPD_BadTowers);
  statusSEPD->set_inputNode("TOWERINFO_CALIB_SEPD_data");
  se->registerSubsystem(statusSEPD);

  // Event Plane
  EventPlaneRecov2* epreco = new EventPlaneRecov2();
  epreco->set_directURL_EventPlaneCalib(input_QVecCalib);
  epreco->set_inputNode("TOWERINFO_COMBINED_SEPD");
  // epreco->set_inputNode("TOWERINFO_CALIB_SEPD_data");
  epreco->Verbosity(Fun4AllBase::VERBOSITY_SOME);
  se->registerSubsystem(epreco);

  // sEPD QA
  sEPD_DataMC_Validation* sepd_validation = new sEPD_DataMC_Validation();
  sepd_validation->Verbosity(1);
  se->registerSubsystem(sepd_validation);

  Fun4AllInputManager* In_global = new Fun4AllDstInputManager("in_global");
  In_global->AddFile(fname_global);
  se->registerInputManager(In_global);

  Fun4AllInputManager* In_jets = new Fun4AllDstInputManager("in_jets");
  In_jets->AddFile(fname_jets);
  se->registerInputManager(In_jets);

  se->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->run(nEvents);
  se->End();

  se->dumpHistos(output);

  CDBInterface::instance()->Print();  // print used DB files
  se->PrintTimer();
  delete se;
  std::cout << "All done!" << std::endl;
  gSystem->Exit(0);
  std::quick_exit(0);
}

# ifndef __CINT__
int main(int argc, const char* const argv[])
{
  const std::vector<std::string> args(argv, argv + argc);

  if (args.size() < 5 || args.size() > 8)
  {
    std::cerr << "usage: " << args[0] << " <input_DST_global> <input_DST_jets> <input_QVecCalib> <input_BadTowers> [output] [nEvents] [dbtag]" << std::endl;
    std::cerr << "  input_DST_global: path to the input global file" << std::endl;
    std::cerr << "  input_DST_jets: path to the input jets file" << std::endl;
    std::cerr << "  input_QVecCalib: sEPD Q Vector Calib CDB" << std::endl;
    std::cerr << "  input_BadTowers: sEPD Bad Towers CDB" << std::endl;
    std::cerr << "  output: (optional) path to the output file (default: 'test.root')" << std::endl;
    std::cerr << "  nEvents: (optional) number of events to process (default: 100)" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: newcdbtag)" << std::endl;
    return 1;  // Indicate error
  }

  const std::string& input_dst_global = args[1];
  const std::string& input_dst_jets = args[2];
  const std::string& input_QVecCalib = args[3];
  const std::string& input_BadTowers = args[4];
  std::string output = (args.size() >= 6) ? args[5] : "test.root";
  int nEvents = (args.size() >= 7) ? std::stoi(args[6]) : 100;
  std::string dbtag = (args.size() >= 8) ? args[7] : "newcdbtag";

  Fun4All_sEPD_DataMC(input_dst_global, input_dst_jets, input_QVecCalib, input_BadTowers, output, nEvents, dbtag);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
# endif
