// c++ includes --
#include <string>
#include <iostream>
#include <memory>
#include <filesystem>
#include <vector>
#include <format>

// root includes --
#include <TSystem.h>
#include <TROOT.h>
#include <TF1.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllBase.h>

#include <phool/recoConsts.h>

#include <caloreco/CaloTowerStatus.h>

#include <calocheck/CaloCheck.h>

R__LOAD_LIBRARY(libCaloCheck.so)

void Fun4All_CaloCheck(const std::string &fname,
                       unsigned int runnumber,
                       const std::string &event_list,
                       int nEvents = 0,
                       float badChi2_IB_threshold = 0.5,
                       const std::string &output = "test.root",
                       const std::string &outputDST = "dst-out.root",
                       const std::string &dbtag = "newcdbtag")
{
  std::cout << "########################" << std::endl;
  std::cout << "Run Parameters" << std::endl;
  std::cout << std::format("input: {}", fname) << std::endl;
  std::cout << std::format("Run: {}", runnumber) << std::endl;
  std::cout << std::format("Event List: {}", event_list) << std::endl;
  std::cout << std::format("badChi2_IB_threshold: {}", badChi2_IB_threshold) << std::endl;
  std::cout << std::format("output: {}", output) << std::endl;
  std::cout << std::format("output DST: {}", outputDST) << std::endl;
  std::cout << std::format("nEvents: {}", nEvents) << std::endl;
  std::cout << std::format("dbtag: {}", dbtag) << std::endl;
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
  se->Verbosity(Fun4AllBase::VERBOSITY_QUIET);

  recoConsts *rc = recoConsts::instance();

  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(Fun4AllBase::VERBOSITY_SOME);

  std::unique_ptr<FlagHandler> flag = std::make_unique<FlagHandler>();
  se->registerSubsystem(flag.release());

  std::unique_ptr<CaloTowerStatus> statusEMC = std::make_unique<CaloTowerStatus>("CEMCSTATUS");
  statusEMC->set_detector_type(CaloTowerDefs::CEMC);
  statusEMC->set_time_cut(1);
  statusEMC->set_badChi2_IB_threshold(badChi2_IB_threshold);
  se->registerSubsystem(statusEMC.release());

  // Calo Check
  std::unique_ptr<CaloCheck> calo_check = std::make_unique<CaloCheck>();
  calo_check->set_event_list(event_list);
  calo_check->set_output(output);
  calo_check->Verbosity(Fun4AllBase::VERBOSITY_SOME);
  se->registerSubsystem(calo_check.release());

  std::unique_ptr<Fun4AllInputManager> In = std::make_unique<Fun4AllDstInputManager>("in");
  In->AddListFile(fname);
  se->registerInputManager(In.release());

  std::unique_ptr<Fun4AllOutputManager> Out = std::make_unique<Fun4AllDstOutputManager>("DSTOUT", outputDST);
  Out->StripNode("TOWERS_ZDC");
  Out->StripNode("TOWERS_HCALIN");
  Out->StripNode("TOWERS_HCALOUT");
  Out->StripNode("TOWERS_SEPD");
  se->registerOutputManager(Out.release());

  // * // // MBD Reconstruction
  // std::unique_ptr<MbdReco> mbdreco = std::make_unique<MbdReco>();
  // se->registerSubsystem(mbdreco.release());

  // // sEPD Reconstruction--Calib Info
  // std::unique_ptr<EpdReco> epdreco = std::make_unique<EpdReco>();
  // se->registerSubsystem(epdreco.release());

  // // ZDC Reconstruction--Calib Info
  // std::unique_ptr<ZdcReco> zdcreco = std::make_unique<ZdcReco>();
  // zdcreco->set_zdc1_cut(0.0);
  // zdcreco->set_zdc2_cut(0.0);
  // se->registerSubsystem(zdcreco.release());

  // // Official vertex storage
  // std::unique_ptr<GlobalVertexReco> gvertex = std::make_unique<GlobalVertexReco>();
  // gvertex->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  // se->registerSubsystem(gvertex.release());

  // setGlobalTag(dbtag);

  // uint64_t default_centrality_run = 54912;
  // std::string cdb_centrality_scale = getCalibration("CentralityScale", default_centrality_run);

  // // Minimum Bias Classifier
  // std::unique_ptr<MinimumBiasClassifier> mb = std::make_unique<MinimumBiasClassifier>();
  // mb->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  // mb->set_mbd_total_charge_cut(2100);
  // mb->setOverwriteScale(cdb_centrality_scale);
  // se->registerSubsystem(mb.release());

  // // Centrality
  // std::unique_ptr<CentralityReco> cent = std::make_unique<CentralityReco>();
  // cent->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  // cent->setOverwriteScale(cdb_centrality_scale);
  // se->registerSubsystem(cent.release());

  // // Event Plane
  // if(do_ep)
  // {
  //   std::unique_ptr<EventPlaneReco> epreco = std::make_unique<EventPlaneReco>();
  //   epreco->set_sepd_epreco(true);
  //   epreco->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  //   se->registerSubsystem(epreco.release());
  // }

  // // sEPD QA
  // std::unique_ptr<sEPDValidation> sepd_validation = std::make_unique<sEPDValidation>();
  // sepd_validation->set_filename(output);
  // sepd_validation->set_condor_mode(condor_mode);
  // sepd_validation->set_do_ep(do_ep);
  // sepd_validation->set_q_vec_corr(q_vec_corr_fname);
  // sepd_validation->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  // se->registerSubsystem(sepd_validation.release());

  se->run(nEvents);
  se->End();

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

  if (args.size() < 4 || args.size() > 9)
  {
    std::cerr << "usage: " << args[0] << " <input_DST_list> <runnumber> <event_list> [nEvents] [badChi2_IB_threshold] [output] [outputDST] [dbtag]" << std::endl;
    std::cerr << "  input_DST: path to the input list file" << std::endl;
    std::cerr << "  runnumber: Run" << std::endl;
    std::cerr << "  event_list: List of events of interest." << std::endl;
    std::cerr << "  nEvents: (optional) number of events to process (default: 100)" << std::endl;
    std::cerr << "  badChi2_IB_threshold: badChi2 IB threshold (default: 0.5)." << std::endl;
    std::cerr << "  output: (optional) path to the output file (default: 'test.root')" << std::endl;
    std::cerr << "  outputDST: (optional) path to the dst output file (default: 'dst-out.root')" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: prodA_2024)" << std::endl;
    return 1;  // Indicate error
  }

  const std::string& input_dst= args[1];
  unsigned int runnumber = static_cast<unsigned int>(std::stoul(args[2]));
  const std::string& event_list = args[3];
  int nEvents = 0;
  float badChi2_IB_threshold = 0.5;
  std::string output = "test.root";
  std::string outputDST = "dst-out.root";
  std::string dbtag = "newcdbtag";

  if (args.size() >= 5)
  {
    nEvents = std::stoi(args[4]);
  }
  if (args.size() >= 6)
  {
    badChi2_IB_threshold = std::stof(args[5]);
  }
  if (args.size() >= 7)
  {
    output = args[6];
  }
  if (args.size() >= 8)
  {
    outputDST = args[7];
  }
  if (args.size() >= 9)
  {
    dbtag = args[8];
  }

  Fun4All_CaloCheck(input_dst, runnumber, event_list, nEvents, badChi2_IB_threshold, output, outputDST, dbtag);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
# endif
