// c++ includes --
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <format>
#include <fstream>

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

#include <eventplaneinfo/EventPlaneReco.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllUtils.h>

#include <phool/recoConsts.h>

#include <sepdvalidation/EventSkip.h>
#include <sepdvalidation/sEPDValidation.h>

#include <jetbackground/BeamBackgroundFilterAndQA.h>
#include <jetbackground/JetBackgroundCut.h>
#include <jetbase/JetCalib.h>

#include "Calo_Calib.C"
#include "HIJetReco.C"

R__LOAD_LIBRARY(libsEPDValidation.so)

void Fun4All_sEPD(const std::string &flist_dst_calofit,
                  const std::string &flist_dst_zdc,
                  const std::string &input_QVecCalib="none",
                  const std::string &output = "test.root",
                  const std::string &output_tree = "tree.root",
                  int nEvents = 100,
                  int nSkip = 0,
                  int event_id = 0,
                  const std::string &dbtag = "newcdbtag")
{

  // Extract runnumber from first file within list
  std::ifstream infile_stream;
  infile_stream.open(flist_dst_calofit, std::ios_base::in);
  std::string filepath;
  getline(infile_stream, filepath);
  std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(filepath);
  int runnumber = runseg.first;
  infile_stream.close();

  std::cout << "########################" << std::endl;
  std::cout << "Run Parameters" << std::endl;
  std::cout << "input calofit: " << flist_dst_calofit << std::endl;
  std::cout << "input zdc: " << flist_dst_zdc << std::endl;
  std::cout << "Run: " << runnumber << std::endl;
  std::cout << "QVec Calib: " << input_QVecCalib << std::endl;
  std::cout << "output: " << output << std::endl;
  std::cout << "output tree: " << output_tree << std::endl;
  std::cout << "nEvents: " << nEvents << std::endl;
  std::cout << "nSkip: " << nSkip << std::endl;
  std::cout << "event_id: " << event_id << std::endl;
  std::cout << "dbtag: " << dbtag << std::endl;
  std::cout << "########################" << std::endl;

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(Fun4AllBase::VERBOSITY_QUIET);

  recoConsts *rc = recoConsts::instance();

  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(Fun4AllBase::VERBOSITY_SOME);

  FlagHandler* flag = new FlagHandler();
  se->registerSubsystem(flag);

  // Event Skip
  EventSkip* evtSkip = new EventSkip();
  evtSkip->set_skip(nSkip);
  evtSkip->set_event_id(event_id);
  evtSkip->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(evtSkip);

  // Calibrate Towers
  Process_Calo_Calib();

  // MBD Reconstruction
  MbdReco* mbdreco = new MbdReco();
  se->registerSubsystem(mbdreco);

  // sEPD Reconstruction--Calib Info
  EpdReco* epdreco = new EpdReco();
  se->registerSubsystem(epdreco);

  // ZDC Reconstruction--Calib Info
  ZdcReco* zdcreco = new ZdcReco();
  zdcreco->set_zdc1_cut(0.0);
  zdcreco->set_zdc2_cut(0.0);
  se->registerSubsystem(zdcreco);

  // Official vertex storage
  GlobalVertexReco* gvertex = new GlobalVertexReco();
  gvertex->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(gvertex);

  // Minimum Bias Classifier
  MinimumBiasClassifier* mb = new MinimumBiasClassifier();
  mb->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  mb->set_mbd_total_charge_cut(2100);
  se->registerSubsystem(mb);

  // Centrality
  CentralityReco* cent = new CentralityReco();
  cent->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(cent);

  // Event Plane
  EventPlaneReco* epreco = new EventPlaneReco();
  epreco->set_directURL_EventPlaneCalib(input_QVecCalib);
  // epreco->Verbosity(Fun4AllBase::VERBOSITY_SOME);
  se->registerSubsystem(epreco);

  // Jet Reco
  // Enable::HIJETS_VERBOSITY = 10;
  HIJETS::do_flow = (input_QVecCalib == "none") ? 0 : 3;
  HIJetReco();

  // Jet Calib R = 0.2
  JetCalib* jetCalib02 = new JetCalib("JetCalib02");
  jetCalib02->set_InputNode("AntiKt_Tower_r02_Sub1");
  jetCalib02->set_OutputNode("AntiKt_Tower_r02_Sub1_calib");
  jetCalib02->set_JetRadius(0.2);
  jetCalib02->set_ZvrtxNode("GlobalVertexMap");
  jetCalib02->set_ApplyZvrtxDependentCalib(true);
  jetCalib02->set_ApplyEtaDependentCalib(true);
  // jetCalib02->Verbosity(1);
  se->registerSubsystem(jetCalib02);

  // Jet Calib R = 0.3
  JetCalib* jetCalib03 = new JetCalib("JetCalib03");
  jetCalib03->set_InputNode("AntiKt_Tower_r03_Sub1");
  jetCalib03->set_OutputNode("AntiKt_Tower_r03_Sub1_calib");
  jetCalib03->set_JetRadius(0.3);
  jetCalib03->set_ZvrtxNode("GlobalVertexMap");
  jetCalib03->set_ApplyZvrtxDependentCalib(true);
  jetCalib03->set_ApplyEtaDependentCalib(true);
  // jetCalib03->Verbosity(1);
  se->registerSubsystem(jetCalib03);

  // sEPD QA
  sEPDValidation* sepd_validation = new sEPDValidation();
  sepd_validation->set_tree_filename(output_tree);
  sepd_validation->set_print_interval(1000);
  sepd_validation->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(sepd_validation);

  Fun4AllInputManager* In = new Fun4AllDstInputManager("calofitting");
  In->AddListFile(flist_dst_calofit);
  se->registerInputManager(In);

  Fun4AllInputManager* In2 = new Fun4AllDstInputManager("zdc");
  In2->AddListFile(flist_dst_zdc);
  se->registerInputManager(In2);

  se->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->run(nEvents+nSkip);
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

  if (args.size() < 3 || args.size() > 10)
  {
    std::cerr << "usage: " << args[0] << " <input_DST> <input_ZDC> [input_QVecCalib] [output] [output_tree] [nEvents] [nSkip] [event_id] [dbtag]" << std::endl;
    std::cerr << "  input_DST: path to the input calo fitting list" << std::endl;
    std::cerr << "  input_ZDC: path to the input ZDC list" << std::endl;
    std::cerr << "  input_QVecCalib: (optional) path to the QVec Calib file (default: 'none')" << std::endl;
    std::cerr << "  output: (optional) path to the output file (default: 'test.root')" << std::endl;
    std::cerr << "  output: (optional) path to the output tree file (default: 'tree.root')" << std::endl;
    std::cerr << "  nEvents: (optional) number of events to process (default: 100)" << std::endl;
    std::cerr << "  nSkip: (optional) number of events to skip (default: 0)" << std::endl;
    std::cerr << "  event_id: (optional) Specific Event to Analyze (default: 0)" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: prodA_2024)" << std::endl;
    std::cerr << "  Do Event Plane: Do official Event Plane reconstruction." << std::endl;
    return 1;  // Indicate error
  }

  const std::string& input_dst= args[1];
  const std::string& input_zdc= args[2];
  std::string input_QVecCalib = "none";
  std::string output = "test.root";
  std::string output_tree = "tree.root";
  int nEvents = 100;
  int nSkip = 0;
  int event_id = 0;
  std::string dbtag = "newcdbtag";

  unsigned int ctr = 3;

  if (args.size() >= ctr+1)
  {
    input_QVecCalib = args[ctr++];
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

  Fun4All_sEPD(input_dst, input_zdc, input_QVecCalib, output, output_tree, nEvents, nSkip, event_id, dbtag);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
# endif
