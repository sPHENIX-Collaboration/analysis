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

#include <eventplaneinfo/EventPlaneReco.h>

#include <phool/recoConsts.h>

#include <sepdvalidation/sEPD_DataMC_Validation.h>

R__LOAD_LIBRARY(libsEPDValidation.so)

void Fun4All_sEPD_DataMC(const std::string& fname_global,
                         const std::string& fname_jets,
                         const std::string& fname_g4hit,
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
  std::cout << "input g4hit: " << fname_g4hit << std::endl;
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

  // sEPD Bad Tower Status - Data
  CaloTowerStatus* statusSEPD_data = new CaloTowerStatus("SEPDSTATUS_data");
  statusSEPD_data->set_detector_type(CaloTowerDefs::SEPD);
  statusSEPD_data->set_directURL_hotMap(input_sEPD_BadTowers);
  statusSEPD_data->set_inputNode("TOWERINFO_CALIB_SEPD_data");
  se->registerSubsystem(statusSEPD_data);

  // sEPD Bad Tower Status - Data+MC
  CaloTowerStatus* statusSEPD_data_mc = new CaloTowerStatus("SEPDSTATUS_data_mc");
  statusSEPD_data_mc->set_detector_type(CaloTowerDefs::SEPD);
  statusSEPD_data_mc->set_directURL_hotMap(input_sEPD_BadTowers);
  statusSEPD_data_mc->set_inputNode("TOWERINFO_COMBINED_SEPD");
  se->registerSubsystem(statusSEPD_data_mc);

  // Event Plane - Data
  EventPlaneReco* epreco_data = new EventPlaneReco("EventPlaneReco_data");
  epreco_data->set_directURL_EventPlaneCalib(input_QVecCalib);
  epreco_data->set_inputNode("TOWERINFO_CALIB_SEPD_data");
  epreco_data->set_EventPlaneInfoNodeName("EventplaneinfoMap_data");
  epreco_data->Verbosity(Fun4AllBase::VERBOSITY_SOME);
  se->registerSubsystem(epreco_data);

  // Event Plane - Data+MC
  EventPlaneReco* epreco_data_mc = new EventPlaneReco("EventPlaneReco_data_mc");
  epreco_data_mc->set_directURL_EventPlaneCalib(input_QVecCalib);
  epreco_data_mc->set_inputNode("TOWERINFO_COMBINED_SEPD");
  epreco_data_mc->set_EventPlaneInfoNodeName("EventplaneinfoMap_data_mc");
  se->registerSubsystem(epreco_data_mc);

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

  Fun4AllInputManager* In_g4hit = new Fun4AllDstInputManager("in_g4hit");
  In_g4hit->AddFile(fname_g4hit);
  se->registerInputManager(In_g4hit);

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

  if (args.size() < 6 || args.size() > 9)
  {
    std::cerr << "usage: " << args[0] << " <input_DST_global> <input_DST_jets> <input_DST_g4hit> <input_QVecCalib> <input_BadTowers> [output] [nEvents] [dbtag]" << std::endl;
    std::cerr << "  input_DST_global: path to the input global file" << std::endl;
    std::cerr << "  input_DST_jets: path to the input jets file" << std::endl;
    std::cerr << "  input_DST_g4hit: path to the input g4hit file" << std::endl;
    std::cerr << "  input_QVecCalib: sEPD Q Vector Calib CDB" << std::endl;
    std::cerr << "  input_BadTowers: sEPD Bad Towers CDB" << std::endl;
    std::cerr << "  output: (optional) path to the output file (default: 'test.root')" << std::endl;
    std::cerr << "  nEvents: (optional) number of events to process (default: 100)" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: newcdbtag)" << std::endl;
    return 1;  // Indicate error
  }

  size_t arg_ctr = 1;
  const std::string& input_dst_global = args[arg_ctr++];
  const std::string& input_dst_jets = args[arg_ctr++];
  const std::string& input_dst_g4hit = args[arg_ctr++];
  const std::string& input_QVecCalib = args[arg_ctr++];
  const std::string& input_BadTowers = args[arg_ctr++];
  std::string output = (args.size() >= arg_ctr+1) ? args[arg_ctr++] : "test.root";
  int nEvents = (args.size() >= arg_ctr+1) ? std::stoi(args[arg_ctr++]) : 100;
  std::string dbtag = (args.size() >= arg_ctr+1) ? args[arg_ctr++] : "newcdbtag";

  Fun4All_sEPD_DataMC(input_dst_global, input_dst_jets, input_dst_g4hit, input_QVecCalib, input_BadTowers, output, nEvents, dbtag);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
# endif
