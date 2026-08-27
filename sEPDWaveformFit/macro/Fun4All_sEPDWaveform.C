#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <ffamodules/CDBInterface.h>
#include <phool/recoConsts.h>
#include <fun4all/Fun4AllUtils.h>

#include <sepdwaveform/sEPDWaveform.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libffarawobjects.so)
R__LOAD_LIBRARY(libsEPDWaveform.so)
#endif

void Fun4All_sEPDWaveform(
    const std::string &input_file = "",
    int nEvents = 10000,
    const std::string &outputFile = "sEPD_templates",
    int runnumber = 0,
    int nsamples = 12,
    bool use_dst_input = false)
{
  if (input_file.empty())
  {
    std::cout << "ERROR: no input_file given" << std::endl;
    gSystem->Exit(1);
  }

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  if (runnumber == 0)
  {
    runnumber = Fun4AllUtils::GetRunSegment(input_file).first;
  }

  recoConsts *rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG", "2025p001");
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  std::cout << "sEPD template generation: run " << runnumber
            << ", CDB tag 2025p001, " << nsamples << " samples" << std::endl;

  sEPDWaveform *sepdWaveform = new sEPDWaveform("sEPDWaveform");
  sepdWaveform->SetDetector("SEPD");
  sepdWaveform->SetPacketRange(9001, 9006);
  sepdWaveform->SetNSamples(nsamples);
  sepdWaveform->SetOutputFileName(outputFile + ".root");

  sepdWaveform->SetFillGeometryHistograms(false);
  sepdWaveform->SetFillGlobalHistograms(true);

  sepdWaveform->SetShowProgress(true);
  sepdWaveform->SetProgressInterval(100);

  se->registerSubsystem(sepdWaveform);

  bool is_list = input_file.find(".list") != std::string::npos
              || input_file.find(".txt") != std::string::npos;
  bool is_dst  = is_list || use_dst_input || input_file.find(".root") != std::string::npos;

  sepdWaveform->SetUseOfflinePackets(is_dst);

  Fun4AllInputManager *in = nullptr;
  if (is_dst)
  {
    std::cout << "Using DST input manager for: " << input_file << std::endl;
    in = new Fun4AllDstInputManager("DSTIN");
  }
  else
  {
    std::cout << "Using PRDF input manager for: " << input_file << std::endl;
    in = new Fun4AllPrdfInputManager("PRDFIN");
  }
  if (is_list)
  {
    in->AddListFile(input_file);
  }
  else
  {
    in->fileopen(input_file);
  }
  se->registerInputManager(in);

  se->run(nEvents);

  se->End();

  delete se;
  gSystem->Exit(0);
}
