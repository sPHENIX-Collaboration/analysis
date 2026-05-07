// c++ includes --
#include <charconv>  // For std::from_chars
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>

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

#include <eventplaneinfo/EventPlaneReco.h>

#include <phool/recoConsts.h>

#include <sepdvalidation/sEPD_DataMC_Validation.h>

#include "Calo_Calib.C"
#include "HIJetReco.C"

R__LOAD_LIBRARY(libsEPDValidation.so)

int GetRun(const std::string& input)
{
    std::string_view sv = input; // You can use the whole path
    std::string_view pattern = "data-";

    size_t pos = sv.find(pattern);
    if (pos == std::string_view::npos) return -1;

    pos += pattern.length(); // Move past "data-"

    size_t end_pos = sv.find('-', pos);
    // If there's no hyphen after the number (e.g., at the end of the string)
    if (end_pos == std::string_view::npos) {
        end_pos = sv.find('.', pos); // Look for the extension instead
    }

    std::string_view result_sv = sv.substr(pos, end_pos - pos);

    int run = 0;
    std::from_chars(result_sv.data(), result_sv.data() + result_sv.size(), run);

    return run;
}

void Fun4All_sEPD_DataMC(const std::string& fname_global,
                         const std::string& fname_jets,
                         const std::string& fname_g4hit,
                         const std::string& fname_calo,
                         const std::string& input_QVecCalib = "default",
                         const std::string& output = "test.root",
                         const std::string& output_tree = "tree.root",
                         int nEvents = 100,
                         double jet_pt_min = 10, /*GeV*/
                         const std::string& dbtag = "newcdbtag")
{
  std::cout << "########################" << std::endl;
  std::cout << "Run Parameters" << std::endl;
  std::cout << "input global: " << fname_global << std::endl;
  std::cout << "input jets: " << fname_jets << std::endl;
  std::cout << "input g4hit: " << fname_g4hit << std::endl;
  std::cout << "input calo: " << fname_calo << std::endl;
  std::cout << "input QVecCalib: " << input_QVecCalib << std::endl;
  std::cout << "output: " << output << std::endl;
  std::cout << "output tree: " << output_tree << std::endl;
  std::cout << "nEvents: " << nEvents << std::endl;
  std::cout << "jet pT min [GeV]: " << jet_pt_min << std::endl;
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

  // std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(fname_global); // does not work as the file naming is not correct
  int runnumber = GetRun(fname_global);

  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(10);

  FlagHandler* flag = new FlagHandler();
  se->registerSubsystem(flag);

  // Calibrate Towers
  Process_Calo_Calib();

  // Event Plane - Data
  EventPlaneReco* epreco_data = new EventPlaneReco("EventPlaneReco_data");
  if (input_QVecCalib != "default")
  {
    epreco_data->set_directURL_EventPlaneCalib(input_QVecCalib);
  }
  epreco_data->set_inputNode("TOWERINFO_CALIB_SEPD_data");
  epreco_data->set_EventPlaneInfoNodeName("EventplaneinfoMap_data");
  // epreco_data->Verbosity(Fun4AllBase::VERBOSITY_SOME);
  se->registerSubsystem(epreco_data);

  // Event Plane - Data+MC
  EventPlaneReco* epreco_data_mc = new EventPlaneReco("EventPlaneReco_data_mc");
  if (input_QVecCalib != "default")
  {
    epreco_data_mc->set_directURL_EventPlaneCalib(input_QVecCalib);
  }
  epreco_data_mc->set_inputNode("TOWERINFO_COMBINED_SEPD");
  epreco_data_mc->set_EventPlaneInfoNodeName("EventplaneinfoMap_data_mc");
  se->registerSubsystem(epreco_data_mc);

  // Jet Reco
  // Enable::HIJETS_VERBOSITY = 10;
  HIJETS::do_flow = 3;
  HIJETS::eventplane_node = "EventplaneinfoMap_data_mc";
  HIJetReco();

  // sEPD QA
  sEPD_DataMC_Validation* sepd_validation = new sEPD_DataMC_Validation();
  sepd_validation->set_jet_pt_min(jet_pt_min);
  sepd_validation->set_tree_filename(output_tree);
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

  Fun4AllInputManager* In_calo = new Fun4AllDstInputManager("in_calo");
  In_calo->AddFile(fname_calo);
  se->registerInputManager(In_calo);

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

  if (args.size() < 5 || args.size() > 11)
  {
    std::cerr << "usage: " << args[0] << " <input_DST_global> <input_DST_jets> <input_DST_g4hit> <input_DST_calo> [input_QVecCalib] [output] [output_tree] [nEvents] [jet_pt_min] [dbtag]" << std::endl;
    std::cerr << "  input_DST_global: path to the input global file" << std::endl;
    std::cerr << "  input_DST_jets: path to the input jets file" << std::endl;
    std::cerr << "  input_DST_g4hit: path to the input g4hit file" << std::endl;
    std::cerr << "  input_DST_calo: path to the input calo file" << std::endl;
    std::cerr << "  input_QVecCalib: sEPD Q Vector Calib CDB (default: (i.e. fetch from CDB))" << std::endl;
    std::cerr << "  output: (optional) path to the output file (default: 'test.root')" << std::endl;
    std::cerr << "  output tree: (optional) path to the output tree (default: 'tree.root')" << std::endl;
    std::cerr << "  nEvents: (optional) number of events to process (default: 100)" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: newcdbtag)" << std::endl;
    return 1;  // Indicate error
  }

  size_t arg_ctr = 1;
  const std::string& input_dst_global = args[arg_ctr++];
  const std::string& input_dst_jets = args[arg_ctr++];
  const std::string& input_dst_g4hit = args[arg_ctr++];
  const std::string& input_dst_calo = args[arg_ctr++];
  const std::string& input_QVecCalib = (args.size() >= arg_ctr+1) ? args[arg_ctr++] : "default";
  std::string output = (args.size() >= arg_ctr+1) ? args[arg_ctr++] : "test.root";
  std::string output_tree = (args.size() >= arg_ctr+1) ? args[arg_ctr++] : "tree.root";
  int nEvents = (args.size() >= arg_ctr+1) ? std::stoi(args[arg_ctr++]) : 100;
  double jet_pt_min = (args.size() >= arg_ctr+1) ? std::stod(args[arg_ctr++]) : 10;
  std::string dbtag = (args.size() >= arg_ctr+1) ? args[arg_ctr++] : "newcdbtag";

  Fun4All_sEPD_DataMC(input_dst_global, input_dst_jets, input_dst_g4hit, input_dst_calo, input_QVecCalib, output, output_tree, nEvents, jet_pt_min, dbtag);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
# endif
