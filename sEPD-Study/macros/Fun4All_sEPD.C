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

#include <mbd/MbdReco.h>
#include <mbd/MbdEvent.h>
#include <epd/EpdReco.h>
#include <zdcinfo/ZdcReco.h>
#include <globalvertex/GlobalVertexReco.h>
#include <centrality/CentralityReco.h>
#include <calotrigger/MinimumBiasClassifier.h>

#include <eventplaneinfo/EventPlaneRecov2.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllBase.h>

#include <phool/recoConsts.h>

#include <sepdvalidation/EventSkip.h>
#include <sepdvalidation/CaloCheck.h>
#include <sepdvalidation/sEPDValidation.h>

#include <CDBUtils.C>

#include <jetbackground/BeamBackgroundFilterAndQA.h>
#include <jetbackground/JetBackgroundCut.h>

#include "Calo_Calib.C"
#include "HIJetReco.C"

R__LOAD_LIBRARY(libsEPDValidation.so)

void Fun4All_sEPD(const std::string &fname,
                  unsigned int runnumber,
                  const std::string &input_QVecCalib="none",
                  const std::string &input_sEPD_BadTowers="none",
                  const std::string &output = "test.root",
                  const std::string &output_tree = "tree.root",
                  int nEvents = 100,
                  int nSkip = 0,
                  int event_id = 0,
                  const std::string &dbtag = "newcdbtag",
                  bool condor_mode = false)
{
  std::cout << "########################" << std::endl;
  std::cout << "Run Parameters" << std::endl;
  std::cout << "input: " << fname << std::endl;
  std::cout << "Run: " << runnumber << std::endl;
  std::cout << "QVec Calib: " << input_QVecCalib << std::endl;
  std::cout << "sEPD Bad Towers: " << input_sEPD_BadTowers << std::endl;
  std::cout << "output: " << output << std::endl;
  std::cout << "output tree: " << output_tree << std::endl;
  std::cout << "nEvents: " << nEvents << std::endl;
  std::cout << "nSkip: " << nSkip << std::endl;
  std::cout << "event_id: " << event_id << std::endl;
  std::cout << "dbtag: " << dbtag << std::endl;
  std::cout << "Condor Mode: " << condor_mode << std::endl;
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

  // Event Skip
  std::unique_ptr<EventSkip> evtSkip = std::make_unique<EventSkip>();
  evtSkip->set_skip(nSkip);
  evtSkip->set_event_id(event_id);
  se->registerSubsystem(evtSkip.release());

  // MBD Reconstruction
  std::unique_ptr<MbdReco> mbdreco = std::make_unique<MbdReco>();
  se->registerSubsystem(mbdreco.release());

  // sEPD Reconstruction--Calib Info
  std::unique_ptr<EpdReco> epdreco = std::make_unique<EpdReco>();
  se->registerSubsystem(epdreco.release());

  // ZDC Reconstruction--Calib Info
  std::unique_ptr<ZdcReco> zdcreco = std::make_unique<ZdcReco>();
  zdcreco->set_zdc1_cut(0.0);
  zdcreco->set_zdc2_cut(0.0);
  se->registerSubsystem(zdcreco.release());

  // Official vertex storage
  std::unique_ptr<GlobalVertexReco> gvertex = std::make_unique<GlobalVertexReco>();
  gvertex->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(gvertex.release());

  setGlobalTag(dbtag);

  uint64_t default_centrality_run = 54912;
  std::string cdb_centrality_scale = getCalibration("CentralityScale", default_centrality_run);

  // Minimum Bias Classifier
  std::unique_ptr<MinimumBiasClassifier> mb = std::make_unique<MinimumBiasClassifier>();
  mb->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  mb->set_mbd_total_charge_cut(2100);
  mb->setOverwriteScale(cdb_centrality_scale);
  se->registerSubsystem(mb.release());

  // Centrality
  std::unique_ptr<CentralityReco> cent = std::make_unique<CentralityReco>();
  cent->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  cent->setOverwriteScale(cdb_centrality_scale);
  se->registerSubsystem(cent.release());

  // Calibrate Towers
  Process_Calo_Calib();

  // sEPD Bad Tower Status
  std::unique_ptr<CaloTowerStatus> statusSEPD = std::make_unique<CaloTowerStatus>("SEPDSTATUS");
  statusSEPD->set_detector_type(CaloTowerDefs::SEPD);
  statusSEPD->set_directURL_hotMap(input_sEPD_BadTowers);
  statusSEPD->set_inputNodePrefix("TOWERINFO_CALIB_");
  se->registerSubsystem(statusSEPD.release());

  // Event Plane
  std::unique_ptr<EventPlaneRecov2> epreco = std::make_unique<EventPlaneRecov2>();
  epreco->set_directURL_EventPlaneCalib(input_QVecCalib);
  epreco->Verbosity(Fun4AllBase::VERBOSITY_SOME);
  se->registerSubsystem(epreco.release());

  // Calo Check
  std::unique_ptr<CaloCheck> calo_check = std::make_unique<CaloCheck>();
  // se->registerSubsystem(calo_check.release());

  // Jet Reco
  // Enable::HIJETS_VERBOSITY = 10;
  HIJETS::do_flow = (input_QVecCalib == "none") ? 0 : 3;
  HIJetReco();

  std::unique_ptr<BeamBackgroundFilterAndQA> filter = std::make_unique<BeamBackgroundFilterAndQA>("BeamBackgroundFilterAndQA");
  filter->Verbosity(Fun4AllBase::VERBOSITY_SOME);
  filter->SetConfig({.debug = false,
                     .doQA = false,
                     .doEvtAbort = false,
                     .filtersToApply = {"StreakSideband"},
                     .null = {},
                     .sideband = {}});
  // se->registerSubsystem(filter.release());

  std::unique_ptr<JetBackgroundCut> jocl = std::make_unique<JetBackgroundCut>("AntiKt_Tower_r02_Sub1");
  jocl->set_input_cemc_tower_node("TOWERINFO_CALIB_CEMC_RETOWER_SUB1");
  jocl->set_input_ohcal_tower_node("TOWERINFO_CALIB_HCALOUT_SUB1");
  // se->registerSubsystem(jocl.release());

  // sEPD QA
  std::unique_ptr<sEPDValidation> sepd_validation = std::make_unique<sEPDValidation>();
  sepd_validation->set_filename(output);
  sepd_validation->set_tree_filename(output_tree);
  sepd_validation->set_condor_mode(condor_mode);
  sepd_validation->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(sepd_validation.release());

  std::unique_ptr<Fun4AllInputManager> In = std::make_unique<Fun4AllDstInputManager>("in");
  In->AddListFile(fname);
  se->registerInputManager(In.release());

  se->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->run(nEvents+nSkip);
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

  if (args.size() < 3 || args.size() > 12)
  {
    std::cerr << "usage: " << args[0] << " <input_DST_list> <runnumber> [input_QVecCalib] [input_sEPD_BadTowers] [output] [output_tree] [nEvents] [nSkip] [event_id] [dbtag] [condor_mode]" << std::endl;
    std::cerr << "  input_DST: path to the input list file" << std::endl;
    std::cerr << "  runnumber: Run" << std::endl;
    std::cerr << "  input_QVecCalib: (optional) path to the QVec Calib file (default: 'none')" << std::endl;
    std::cerr << "  input_sEPD_BadTowers: (optional) path to the sEPD Bad Towers file (default: 'none')" << std::endl;
    std::cerr << "  output: (optional) path to the output file (default: 'test.root')" << std::endl;
    std::cerr << "  output: (optional) path to the output tree file (default: 'tree.root')" << std::endl;
    std::cerr << "  nEvents: (optional) number of events to process (default: 100)" << std::endl;
    std::cerr << "  nSkip: (optional) number of events to skip (default: 0)" << std::endl;
    std::cerr << "  event_id: (optional) Specific Event to Analyze (default: 0)" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: prodA_2024)" << std::endl;
    std::cerr << "  Condor Mode: set condor mode for efficient output file." << std::endl;
    std::cerr << "  Do Event Plane: Do official Event Plane reconstruction." << std::endl;
    return 1;  // Indicate error
  }

  const std::string& input_dst= args[1];
  unsigned int runnumber = static_cast<unsigned int>(std::stoul(args[2]));
  std::string input_QVecCalib = "none";
  std::string input_sEPD_BadTowers = "none";
  std::string output = "test.root";
  std::string output_tree = "tree.root";
  int nEvents = 100;
  int nSkip = 0;
  int event_id = 0;
  std::string dbtag = "newcdbtag";
  bool condor_mode = false;

  unsigned int ctr = 3;

  if (args.size() >= ctr+1)
  {
    input_QVecCalib = args[ctr++];
  }
  if (args.size() >= ctr+1)
  {
    input_sEPD_BadTowers = args[ctr++];
  }
  if (args.size() >= ctr+1)
  {
    output = args[ctr++];
  }
  if (args.size() >= ctr+1)
  {
    output_tree = args[ctr++];
  }
  if (args.size() >= ctr+1)
  {
    nEvents = std::stoi(args[ctr++]);
  }
  if (args.size() >= ctr+1)
  {
    nSkip = std::stoi(args[ctr++]);
  }
  if (args.size() >= ctr+1)
  {
    event_id = std::stoi(args[ctr++]);
  }
  if (args.size() >= ctr+1)
  {
    dbtag = args[ctr++];
  }
  if (args.size() >= ctr+1)
  {
    condor_mode = std::stoi(args[ctr++]);
  }

  Fun4All_sEPD(input_dst, runnumber, input_QVecCalib, input_sEPD_BadTowers, output, output_tree, nEvents, nSkip, event_id, dbtag, condor_mode);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
# endif
