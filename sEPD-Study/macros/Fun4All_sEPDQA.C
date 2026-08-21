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
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllUtils.h>

#include <phool/recoConsts.h>

#include <calotrigger/TriggerRunInfoReco.h>

#include <sepdvalidation/EventSkip.h>
#include <sepdvalidation/EventQA.h>
#include <sepdvalidation/CaloQA.h>
#include <sepdvalidation/GlobalQA.h>

#include <jetbackground/RetowerCEMC.h>

#include <treefiller/TreeFiller.h>

#include "Calo_Calib.C"

R__LOAD_LIBRARY(libg4detectors_io.so)
R__LOAD_LIBRARY(libjetbackground.so)
R__LOAD_LIBRARY(libsEPDValidation.so)
R__LOAD_LIBRARY(libTreeFiller.so)

void Fun4All_sEPDQA(const std::string &flist_dst_calofit = "DST_CALOFITTING_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                    const std::string &flist_dst_zdc = "/direct/sphenix+tg+tg01/jets/anarde/run3auau/ZDC/68144/DST_ZDC_CALIB_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                    const std::string &flist_dst_sepd = "DST_SEPD_RAW_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                    const std::string &output_tree = "tree.root",
                    int nEvents = 100,
                    const std::string &dbtag = "newcdbtag")
{
  // Extract runnumber and segment from first file within list
  int runnumber;
  int segment;
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
  std::cout << "input calofit: " << flist_dst_calofit << std::endl;
  std::cout << "input zdc: " << flist_dst_zdc << std::endl;
  std::cout << "input sepd: " << flist_dst_sepd << std::endl;
  std::cout << "Run: " << runnumber << std::endl;
  std::cout << "Segment: " << segment << std::endl;
  std::cout << "output tree: " << output_tree << std::endl;
  std::cout << "nEvents: " << nEvents << std::endl;
  std::cout << "dbtag: " << dbtag << std::endl;
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

  // Calibrate Towers
  Process_Calo_Calib();

  RetowerCEMC *rcemc = new RetowerCEMC();
  rcemc->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  rcemc->set_towerinfo(true);
  rcemc->set_frac_cut(1);  // fraction of retower that must be masked to mask the full retower. 1=do not apply extra masking
  rcemc->set_do_rescale(false); // scale the retowered towers up to account for dead area?
  rcemc->set_towerNodePrefix("TOWERINFO_CALIB");
  se->registerSubsystem(rcemc);

  // MBD Reconstruction
  MbdReco* mbdreco = new MbdReco();
  se->registerSubsystem(mbdreco);

  CaloTowerDefs::BuilderType buildertype = CaloTowerDefs::kPRDFTowerv4;

  // sEPD Reconstruction--Calib Info: Packets -> TOWERS_SEPD
  CaloTowerBuilder* caEPD = new CaloTowerBuilder("SEPDBUILDER");
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

  // Event QA
  EventQA* event_qa = new EventQA();
  event_qa->set_do_hist(false);
  event_qa->set_cent_max(100);
  event_qa->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(event_qa);

  // Calo QA
  CaloQA* calo_qa = new CaloQA();
  calo_qa->set_do_hist(false);
  calo_qa->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(calo_qa);

  // Global QA
  GlobalQA* global_qa = new GlobalQA();
  global_qa->set_do_ep(false);
  global_qa->set_do_sepd(true);
  global_qa->set_do_mbd(true);
  global_qa->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(global_qa);

  // Tree Filler
  TreeFiller* tree_filler = TreeFiller::instance();
  tree_filler->setOutfileName(output_tree);
  tree_filler->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(tree_filler);

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

  se->run(nEvents);
  se->End();

  se->dumpHistos(output_tree);

  CDBInterface::instance()->Print();  // print used DB files
  se->PrintTimer();
  delete se;
  std::cout << "All done!" << std::endl;
  gSystem->Exit(0);
  std::quick_exit(0);
}
