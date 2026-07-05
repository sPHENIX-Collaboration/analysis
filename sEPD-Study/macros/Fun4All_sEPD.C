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

#include <caloreco/CaloTowerBuilder.h>

#include <mbd/MbdReco.h>
#include <mbd/MbdEvent.h>
#include <epd/EpdReco.h>
#include <globalvertex/GlobalVertexReco.h>
#include <centrality/CentralityReco.h>
#include <calotrigger/MinimumBiasClassifier.h>

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

#include <jetbase/JetCalib.h>

#include "Calo_Calib.C"
#include "HIJetReco.C"

R__LOAD_LIBRARY(libg4detectors_io.so)
R__LOAD_LIBRARY(libsEPDValidation.so)

void Fun4All_sEPD(const std::string &flist_dst_calofit = "DST_CALOFITTING_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                  const std::string &flist_dst_zdc = "/direct/sphenix+tg+tg01/jets/anarde/run3auau/ZDC/68144/DST_ZDC_CALIB_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                  const std::string &flist_dst_sepd = "DST_SEPD_RAW_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                  const std::string &input_QVecCalib="default",
                  const std::string &output = "test.root",
                  const std::string &output_tree = "tree.root",
                  int nEvents = 100,
                  int nSkip = 0,
                  int event_id = 0,
                  const std::string &dbtag = "newcdbtag")
{

  // Extract runnumber from first file within list
  int runnumber;
  bool isFileList = true;
  // single file
  if (flist_dst_calofit.ends_with(".root"))
  {
    std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(flist_dst_calofit);
    runnumber = runseg.first;
    isFileList = false;
  }
  // list of files
  else
  {
    std::ifstream infile_stream(flist_dst_calofit);
    if (!infile_stream) {
      std::cout << "Error: Could not open file list " << flist_dst_calofit << std::endl;
      return;
    }
    std::string filepath;
    getline(infile_stream, filepath);
    std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(filepath);
    runnumber = runseg.first;
    infile_stream.close();
  }

  std::cout << "########################" << std::endl;
  std::cout << "Run Parameters" << std::endl;
  std::cout << "input calofit: " << flist_dst_calofit << std::endl;
  std::cout << "input zdc: " << flist_dst_zdc << std::endl;
  std::cout << "input sepd: " << flist_dst_sepd << std::endl;
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

  CaloTowerDefs::BuilderType buildertype = CaloTowerDefs::kPRDFTowerv4;

  // sEPD Reconstruction--Calib Info: Packets -> TOWERS_SEPD
  CaloTowerBuilder *caEPD = new CaloTowerBuilder("SEPDBUILDER");
  caEPD->set_detector_type(CaloTowerDefs::SEPD);
  caEPD->set_builder_type(buildertype);
  caEPD->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  caEPD->set_nsamples(12);
  caEPD->set_offlineflag();
  se->registerSubsystem(caEPD);

  // sEPD Reconstruction--Calib Info
  EpdReco* epdreco = new EpdReco();
  se->registerSubsystem(epdreco);

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

  // Jet Reco
  // Enable::HIJETS_VERBOSITY = 10;
  HIJETS::eventplane_custom_calib = input_QVecCalib;
  HIJETS::do_flow = 3;
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
  sepd_validation->set_print_interval(10000);
  sepd_validation->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(sepd_validation);

  Fun4AllInputManager* In = new Fun4AllDstInputManager("calofitting");
  if (isFileList)
  {
    In->AddListFile(flist_dst_calofit);
  }
  else
  {
    In->AddFile(flist_dst_calofit);
  }
  se->registerInputManager(In);

  Fun4AllInputManager* In2 = new Fun4AllDstInputManager("zdc");
  if (isFileList)
  {
    In2->AddListFile(flist_dst_zdc);
  }
  else
  {
    In2->AddFile(flist_dst_zdc);
  }
  se->registerInputManager(In2);

  Fun4AllInputManager* In3 = new Fun4AllDstInputManager("sepd");
  if (isFileList)
  {
    In3->AddListFile(flist_dst_sepd);
  }
  else
  {
    In3->AddFile(flist_dst_sepd);
  }
  se->registerInputManager(In3);

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

  if (args.size() < 4 || args.size() > 11)
  {
    std::cerr << "usage: " << args[0] << " <input_DST> <input_ZDC> <input_SEPD> [input_QVecCalib] [output] [output_tree] [nEvents] [nSkip] [event_id] [dbtag]" << std::endl;
    std::cerr << "  input_DST: path to the input calo fitting list" << std::endl;
    std::cerr << "  input_ZDC: path to the input ZDC list" << std::endl;
    std::cerr << "  input_SEPD: path to the input SEPD list" << std::endl;
    std::cerr << "  input_QVecCalib: (optional) path to the QVec Calib file (default: (i.e. fetch from CDB))" << std::endl;
    std::cerr << "  output: (optional) path to the output file (default: 'test.root')" << std::endl;
    std::cerr << "  output: (optional) path to the output tree file (default: 'tree.root')" << std::endl;
    std::cerr << "  nEvents: (optional) number of events to process (default: 100)" << std::endl;
    std::cerr << "  nSkip: (optional) number of events to skip (default: 0)" << std::endl;
    std::cerr << "  event_id: (optional) Specific Event to Analyze (default: 0)" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: newcdbtag)" << std::endl;
    return 1;  // Indicate error
  }

  size_t arg_ctr = 1;
  const std::string& input_dst = args[arg_ctr++];
  const std::string& input_zdc = args[arg_ctr++];
  const std::string& input_sepd = args[arg_ctr++];
  const std::string& input_QVecCalib = (args.size() >= arg_ctr+1) ? args[arg_ctr++] : "default";
  std::string output = (args.size() >= arg_ctr+1) ? args[arg_ctr++] : "test.root";
  std::string output_tree = (args.size() >= arg_ctr+1) ? args[arg_ctr++] : "tree.root";
  int nEvents = (args.size() >= arg_ctr+1) ? std::stoi(args[arg_ctr++]) : 100;
  int nSkip = (args.size() >= arg_ctr+1) ? std::stoi(args[arg_ctr++]) : 0;
  int event_id = (args.size() >= arg_ctr+1) ? std::stoi(args[arg_ctr++]) : 0;
  std::string dbtag = (args.size() >= arg_ctr+1) ? args[arg_ctr++] : "newcdbtag";

  Fun4All_sEPD(input_dst, input_zdc, input_sepd, input_QVecCalib, output, output_tree, nEvents, nSkip, event_id, dbtag);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
# endif
