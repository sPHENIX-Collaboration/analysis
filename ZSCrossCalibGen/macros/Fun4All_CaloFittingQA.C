#ifndef FUN4ALL_YEAR2_FITTING_C
#define FUN4ALL_YEAR2_FITTING_C

#include "Calo_Fitting.C"
#include <QA.C>

#include <calotrigger/TriggerRunInfoReco.h>

#include <calovalid/CaloFittingQA.h>

#include <calopacketskimmer/CaloPacketSkimmer.h>

#include <mbd/MbdReco.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/SubsysReco.h>

#include <phool/recoConsts.h>

#include <TSystem.h>

#include <fstream>

R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalovalid.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libCaloPacketSkimmer.so)

void Fun4All_CaloFittingQA(int nEvents = 100,
                           const std::string &inlist = "test.list",
                           const std::string &outfile_hist = "test.root",
                           const std::string &dbtag = "newcdbtag")
{
  gSystem->Load("libg4dst.so");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);
  se->VerbosityDownscale(1000);

  recoConsts *rc = recoConsts::instance();

  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);

  CDBInterface::instance()->Verbosity(1);

  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  // Get info from DB and store in DSTs
  TriggerRunInfoReco *triggerinfo = new TriggerRunInfoReco();
  se->registerSubsystem(triggerinfo);

  CaloPacketSkimmer *calopacket = new CaloPacketSkimmer();
  calopacket->set_all_detectors_off();
  calopacket->set_detector_on(CaloTowerDefs::CEMC);
  calopacket->set_detector_on(CaloTowerDefs::HCALIN);
  calopacket->set_detector_on(CaloTowerDefs::HCALOUT);
  se->registerSubsystem(calopacket);

  Process_Calo_Fitting();

  ///////////////////////////////////
  // Validation
  CaloFittingQA *ca = new CaloFittingQA("CaloFittingQA");
  se->registerSubsystem(ca);

  Fun4AllInputManager *In = nullptr;
  std::ifstream infile;
  infile.open(inlist);
  int iman = 0;
  std::string line;
  if (infile.is_open())
  {
    bool first{true};
    while (std::getline(infile, line))
    {
      if (line[0] == '#')
      {
        std::cout << "found commented out line " << line << std::endl;
        continue;
      }
      if (first)
      {
        std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(line);
        int runnumber = runseg.first;
        rc->set_uint64Flag("TIMESTAMP", runnumber);
        first = false;
      }

      std::cout << line << std::endl;
      std::string magname = "DSTin_" + std::to_string(iman);
      In = new Fun4AllDstInputManager(magname);
      In->Verbosity(1);
      In->AddFile(line);
      se->registerInputManager(In);
      iman++;
    }
    infile.close();
  }
  if (iman == 0)
  {
    std::cout << "No files in filelist" << std::endl;
    gSystem->Exit(1);
  }
  se->run(nEvents);
  se->End();

  QAHistManagerDef::saveQARootFile(outfile_hist);

  CDBInterface::instance()->Print();  // print used DB files
  se->PrintTimer();
  delete se;
  std::cout << "All done!" << std::endl;
  gSystem->Exit(0);
}
#endif
