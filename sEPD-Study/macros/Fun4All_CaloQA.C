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
#include <zdcinfo/ZdcReco.h>
#include <globalvertex/GlobalVertexReco.h>
#include <centrality/CentralityReco.h>
#include <calotrigger/MinimumBiasClassifier.h>
#include <jetbackground/RetowerCEMC.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllUtils.h>

#include <phool/recoConsts.h>

#include <calotrigger/TriggerRunInfoReco.h>

#include <sepdvalidation/EventQA.h>
#include <sepdvalidation/CaloQA.h>

#include "Calo_Calib.C"

R__LOAD_LIBRARY(libg4detectors_io.so)
R__LOAD_LIBRARY(libsEPDValidation.so)

void Fun4All_CaloQA(const std::string &flist_dst_calofit = "DST_CALOFITTING_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                    // const std::string &flist_dst_zdc = "/direct/sphenix+tg+tg01/jets/anarde/run3auau/ZDC/68144/DST_ZDC_CALIB_run3auau_pro001_pcdb001_v001-00068144-00000.root",
                    const std::string &flist_dst_zdc = "",
                    const std::string &output = "test.root",
                    int nEvents = 100,
                    const std::string &dbtag = "newcdbtag")
{
  bool is_single_dst = flist_dst_zdc.empty() || flist_dst_zdc == "none";

  // Extract runnumber and segment from first file within list
  int runnumber = 0;
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
  if (is_single_dst)
  {
    std::cout << "input single DST: " << flist_dst_calofit << std::endl;
  }
  else
  {
    std::cout << "input calofit: " << flist_dst_calofit << std::endl;
    std::cout << "input zdc: " << flist_dst_zdc << std::endl;
  }
  std::cout << "output: " << output << std::endl;
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

  // Event QA
  EventQA* event_qa = new EventQA();
  event_qa->set_do_tree(false);
  event_qa->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(event_qa);

  // Calo QA
  CaloQA* calo_qa = new CaloQA();
  calo_qa->set_do_retower(false);
  calo_qa->set_do_tree(false);
  calo_qa->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(calo_qa);

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
        {"zdc", flist_dst_zdc}};

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

// ----------------------------------------------------------------------------
// Overloaded Wrapper for Single DST Input
// ----------------------------------------------------------------------------
void Fun4All_CaloQA(const std::string &input_dst,
                    const std::string &output,
                    int nEvents,
                    const std::string &dbtag)
{
  Fun4All_CaloQA(input_dst, /*flist_dst_zdc=*/"", output, nEvents, dbtag);
}
