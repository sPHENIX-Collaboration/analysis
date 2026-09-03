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

#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllUtils.h>

#include <phool/recoConsts.h>

#include <calotrigger/TriggerRunInfoReco.h>

#include <sepdvalidation/EventSkip.h>
#include <sepdvalidation/EventQA.h>
#include <sepdvalidation/GlobalQA.h>
#include <sepdvalidation/CaloQA.h>
#include <sepdvalidation/JetValidationv3.h>
#include <sepdvalidation/RandomConeValidation.h>

#include <treefiller/TreeFiller.h>

#include "Calo_Calib.C"
#include "HIJetReco.C"

R__LOAD_LIBRARY(libg4detectors_io.so)
R__LOAD_LIBRARY(libsEPDValidation.so)
R__LOAD_LIBRARY(libTreeFiller.so)

void Fun4All_BkgSub(const std::string &flist_dst_calofit = "DST_CALOFITTING_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                    const std::string &flist_dst_zdc = "",
                    const std::string &flist_dst_sepd = "",
                    // const std::string &flist_dst_zdc = "/direct/sphenix+tg+tg01/jets/anarde/run3auau/ZDC/68144/DST_ZDC_CALIB_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                    // const std::string &flist_dst_sepd = "DST_SEPD_RAW_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                    const std::string &input_QVecCalib = "default",
                    const std::string &output = "test.root",
                    const std::string &output_tree = "tree.root",
                    int do_flow = 3,
                    int nEvents = 100,
                    int nSkip = 0,
                    int event_id = 0,
                    const std::string &dbtag = "newcdbtag",
                    const std::string &eta_calib_direct_path = "",
                    const std::string &event_list = "",
                    bool do_rcone = false,
                    bool do_mult = false,
                    const std::string &output_dst = "",
                    bool do_neg_energy_threshold = true,
                    float neg_energy_threshold = -2.0F)
{
  bool is_single_dst = flist_dst_zdc.empty() || flist_dst_zdc == "none";

  // Extract runnumber and segment from first file within list
  int runnumber = 0;
  int segment = 0;
  bool isFileList = true;
  // single file
  if (flist_dst_calofit.ends_with(".root"))
  {
    std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(flist_dst_calofit);
    runnumber = runseg.first;
    segment = runseg.second;
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
    segment = runseg.second;
    infile_stream.close();
  }

  std::cout << "########################" << std::endl;
  std::cout << "Run Parameters" << std::endl;
  if (is_single_dst)
  {
    std::cout << "input single DST: " << flist_dst_calofit << std::endl;
  }
  else
  {
    std::cout << "input calofit: " << flist_dst_calofit << std::endl;
    std::cout << "input zdc: " << flist_dst_zdc << std::endl;
    std::cout << "input sepd: " << flist_dst_sepd << std::endl;
  }
  std::cout << "Run: " << runnumber << std::endl;
  std::cout << "QVec Calib: " << input_QVecCalib << std::endl;
  std::cout << "output: " << output << std::endl;
  std::cout << "output tree: " << output_tree << std::endl;
  std::cout << "output dst: " << output_dst << std::endl;
  std::cout << "do_flow: " << do_flow << std::endl;
  std::cout << "nEvents: " << nEvents << std::endl;
  std::cout << "nSkip: " << nSkip << std::endl;
  std::cout << "event_id: " << event_id << std::endl;
  std::cout << "event_list: " << event_list << std::endl;
  std::cout << "dbtag: " << dbtag << std::endl;
  std::cout << "eta_calib_direct_path: " << eta_calib_direct_path << std::endl;
  std::cout << "do_rcone: " << do_rcone << std::endl;
  std::cout << "do_mult: " << do_mult << std::endl;
  std::cout << "do_neg_energy_threshold: " << do_neg_energy_threshold << std::endl;
  std::cout << "neg_energy_threshold: " << neg_energy_threshold << std::endl;
  std::cout << "########################" << std::endl;

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(Fun4AllBase::VERBOSITY_SOME);
  se->VerbosityDownscale(1000);

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
  if (!event_list.empty())
  {
    evtSkip->set_event_ids(event_list);
  }
  else if (event_id != 0)
  {
    evtSkip->set_event_id(event_id);
  }
  evtSkip->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(evtSkip);

  // Calibrate Towers
  CaloCalib::do_neg_energy_threshold = do_neg_energy_threshold;
  CaloCalib::neg_energy_threshold = neg_energy_threshold;
  Process_Calo_Calib();

  // MBD Reconstruction
  MbdReco* mbdreco = new MbdReco();
  se->registerSubsystem(mbdreco);

  CaloTowerDefs::BuilderType buildertype = CaloTowerDefs::kPRDFTowerv4;

  // sEPD Reconstruction--Calib Info: Packets -> TOWERS_SEPD
  if (do_flow == 3)
  {
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
  }

  // Official vertex storage
  GlobalVertexReco* gvertex = new GlobalVertexReco();
  gvertex->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(gvertex);

  // Trigger Info Reco
  TriggerRunInfoReco* trig = new TriggerRunInfoReco();
  trig->Verbosity(1);
  se->registerSubsystem(trig);

  // Minimum Bias Classifier
  MinimumBiasClassifier* mb = new MinimumBiasClassifier();
  mb->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  mb->set_mbd_total_charge_cut(2100);
  se->registerSubsystem(mb);

  // Centrality
  CentralityReco* cent = new CentralityReco();
  cent->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(cent);

  bool do_detailed = !event_list.empty() || event_id != 0;

  // Event QA
  EventQA* event_qa = new EventQA();
  if (do_detailed)
  {
    event_qa->set_do_hist(false);
  }
  event_qa->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(event_qa);

  // Jet Reco
  // Enable::HIJETS_VERBOSITY = 10;
  HIJETS::eventplane_custom_calib = input_QVecCalib;
  HIJETS::do_flow = do_flow;
  if (eta_calib_direct_path != "none" && !eta_calib_direct_path.empty())
  {
    HIJETS::eta_calib_direct_path = eta_calib_direct_path;
  }
  Enable::HIJETS_TOWER_MULTSUB = do_mult;
  Enable::HIJETS_TOWER_NOBKG = true;
  HIJetReco();

  // Calo QA
  CaloQA* calo_qa = new CaloQA();
  calo_qa->set_do_hist(false);
  if (do_detailed)
  {
    calo_qa->set_do_detailed(true);
    calo_qa->set_do_iter(true);
    calo_qa->set_do_mult(do_mult);
  }
  calo_qa->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(calo_qa);

  // Global QA
  GlobalQA* global_qa = new GlobalQA();
  global_qa->set_do_ep(do_flow);
  global_qa->set_do_sepd(false);
  global_qa->set_do_mbd(false);
  global_qa->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(global_qa);

  // Jet Validation
  JetValidationv3* jet_validation = new JetValidationv3();
  jet_validation->set_do_mult(do_mult);
  if (do_detailed)
  {
    jet_validation->set_do_detailed(true);
  }
  jet_validation->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(jet_validation);

  // Random Cone Validation
  if (do_rcone)
  {
    RandomConeValidation* random_cone_validation = new RandomConeValidation();
    if (do_detailed)
    {
      random_cone_validation->set_do_detailed(true);
    }
    random_cone_validation->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
    se->registerSubsystem(random_cone_validation);
  }

  // Tree Filler
  TreeFiller* tree_filler = TreeFiller::instance();
  tree_filler->setOutfileName(output_tree);
  tree_filler->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(tree_filler);

  // Output DST for detailed events (event_id or event_list)
  if (do_detailed && output_dst != "none")
  {
    std::string dst_name = output_dst;
    if (dst_name.empty())
    {
      dst_name = std::format("dst-{:08d}-{:05d}.root", runnumber, segment);
    }

    Fun4AllDstOutputManager *dst_out = new Fun4AllDstOutputManager("DSTOUT", dst_name);
    dst_out->SplitLevel(99);
    dst_out->AddNode("Sync");
    dst_out->AddNode("EventHeader");
    dst_out->AddNode("14001");
    dst_out->AddNode("MbdRawContainer");
    dst_out->AddNode("TOWERS_CEMC");
    dst_out->AddNode("TOWERS_HCALIN");
    dst_out->AddNode("TOWERS_HCALOUT");
    dst_out->AddNode("Zdcinfo");
    dst_out->AddNode("9001");
    dst_out->AddNode("9002");
    dst_out->AddNode("9003");
    dst_out->AddNode("9004");
    dst_out->AddNode("9005");
    dst_out->AddNode("9006");
    se->registerOutputManager(dst_out);
  }

  if (is_single_dst)
  {
    Fun4AllInputManager* in = new Fun4AllDstInputManager("DSTin");
    if (isFileList)
    {
      in->AddListFile(flist_dst_calofit);
    }
    else
    {
      in->AddFile(flist_dst_calofit);
    }
    se->registerInputManager(in);
  }
  else
  {
    const std::vector<std::pair<std::string, std::string>> input_files = {
        {"calofitting", flist_dst_calofit},
        {"zdc", flist_dst_zdc},
        {"sepd", flist_dst_sepd}};

    for (const auto& [name, filepath] : input_files)
    {
      Fun4AllInputManager* in = new Fun4AllDstInputManager(name);
      if (isFileList)
      {
        in->AddListFile(filepath);
      }
      else
      {
        in->AddFile(filepath);
      }
      se->registerInputManager(in);
    }
  }

  se->run(nEvents+nSkip);
  se->End();

  if (do_detailed)
  {
    se->dumpHistos(output_tree);
  }
  else
  {
    se->dumpHistos(output);
  }

  CDBInterface::instance()->Print();  // print used DB files
  se->PrintTimer();
  delete se;
  std::cout << "All done!" << std::endl;
  gSystem->Exit(0);
  std::quick_exit(0);
}

// ----------------------------------------------------------------------------
// Overloaded Wrapper for Single DST Input
// ----------------------------------------------------------------------------
void Fun4All_BkgSub(const std::string &input_dst,
                    const std::string &input_QVecCalib,
                    const std::string &output,
                    const std::string &output_tree,
                    int do_flow,
                    int nEvents,
                    int nSkip,
                    int event_id,
                    const std::string &dbtag,
                    const std::string &eta_calib_direct_path,
                    const std::string &event_list,
                    bool do_rcone,
                    bool do_mult,
                    const std::string &output_dst,
                    bool do_neg_energy_threshold = true,
                    float neg_energy_threshold = -2.0F)
{
  Fun4All_BkgSub(input_dst, /*flist_dst_zdc=*/"", /*flist_dst_sepd=*/"",
                 input_QVecCalib, output, output_tree, do_flow, nEvents, nSkip, event_id,
                 dbtag, eta_calib_direct_path, event_list, do_rcone, do_mult, output_dst,
                 do_neg_energy_threshold, neg_energy_threshold);
}
