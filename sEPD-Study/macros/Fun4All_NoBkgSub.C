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

#include <calotrigger/TriggerRunInfoReco.h>

#include <sepdvalidation/JetValidationv2.h>

#include <jetbase/JetCalib.h>

#include "Calo_Calib.C"
#include "NoBkgdSubJetReco.C"

R__LOAD_LIBRARY(libg4detectors_io.so)
R__LOAD_LIBRARY(libsEPDValidation.so)

void Fun4All_NoBkgSub(const std::string &flist_dst_calofit = "DST_CALOFITTING_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                      const std::string &flist_dst_zdc = "/direct/sphenix+tg+tg01/jets/anarde/run3auau/ZDC/68144/DST_ZDC_CALIB_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                      const std::string &output = "test.root",
                      const std::string &output_tree = "tree.root",
                      int nEvents = 100,
                      int logInterval = 10000,
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
  std::cout << "Run: " << runnumber << std::endl;
  std::cout << "output: " << output << std::endl;
  std::cout << "output tree: " << output_tree << std::endl;
  std::cout << "nEvents: " << nEvents << std::endl;
  std::cout << "Log Interval: " << logInterval << std::endl;
  std::cout << "dbtag: " << dbtag << std::endl;
  std::cout << "########################" << std::endl;

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(Fun4AllBase::VERBOSITY_SOME);
  se->VerbosityDownscale(logInterval);

  recoConsts *rc = recoConsts::instance();

  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(Fun4AllBase::VERBOSITY_SOME);

  FlagHandler* flag = new FlagHandler();
  se->registerSubsystem(flag);

  // Calibrate Towers
  Process_Calo_Calib();

  // MBD Reconstruction
  MbdReco* mbdreco = new MbdReco();
  se->registerSubsystem(mbdreco);

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

  // Jet Reco
  Enable::NSJETS_TOWER = true;
  Enable::NSJETS_TRACK = false;

  NoBkgdSubJetReco();

  // Jet Calib R = 0.2
  JetCalib* jetCalib02 = new JetCalib("JetCalib02");
  jetCalib02->set_InputNode("AntiKt_Tower_r02");
  jetCalib02->set_OutputNode("AntiKt_Tower_r02_calib");
  jetCalib02->set_JetRadius(0.2);
  jetCalib02->set_ZvrtxNode("GlobalVertexMap");
  jetCalib02->set_ApplyZvrtxDependentCalib(true);
  jetCalib02->set_ApplyEtaDependentCalib(true);
  // jetCalib02->Verbosity(1);
  se->registerSubsystem(jetCalib02);

  // Jet QA
  JetValidationv2* jet_validation = new JetValidationv2();
  jet_validation->set_tree_filename(output_tree);
  jet_validation->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(jet_validation);

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
