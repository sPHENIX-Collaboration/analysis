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
#include <globalvertex/GlobalVertexReco.h>
#include <jetbackground/RetowerCEMC.h>

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
#include <sepdvalidation/CaloCheck.h>

#include <CDBUtils.C>

#include "Calo_Calib.C"

R__LOAD_LIBRARY(libsEPDValidation.so)

void Fun4All_CaloCheck(const std::string &fname,
                  int event_id = 0,
                  const std::string &dbtag = "newcdbtag")
{
  std::cout << "########################" << std::endl;
  std::cout << "Run Parameters" << std::endl;
  std::cout << "input: " << fname << std::endl;
  std::cout << "event_id: " << event_id << std::endl;
  std::cout << "dbtag: " << dbtag << std::endl;
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

  std::pair<int, int> runnumber_segment = Fun4AllUtils::GetRunSegment(fname);
  uint64_t runnumber = static_cast<uint64_t>(runnumber_segment.first);

  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(Fun4AllBase::VERBOSITY_SOME);

  std::unique_ptr<FlagHandler> flag = std::make_unique<FlagHandler>();
  se->registerSubsystem(flag.release());

  // Event Skip
  std::unique_ptr<EventSkip> evtSkip = std::make_unique<EventSkip>();
  evtSkip->set_event_id(event_id);
  se->registerSubsystem(evtSkip.release());

  // MBD Reconstruction
  std::unique_ptr<MbdReco> mbdreco = std::make_unique<MbdReco>();
  se->registerSubsystem(mbdreco.release());

  // Official vertex storage
  std::unique_ptr<GlobalVertexReco> gvertex = std::make_unique<GlobalVertexReco>();
  gvertex->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(gvertex.release());

  // Calibrate Towers
  Process_Calo_Calib();

  std::unique_ptr<RetowerCEMC> rcemc = std::make_unique<RetowerCEMC>();
  rcemc->set_towerinfo(true);
  rcemc->set_frac_cut(0.5);  // fraction of retower that must be masked to mask the full retower
  rcemc->set_towerNodePrefix("TOWERINFO_CALIB");
  se->registerSubsystem(rcemc.release());

  // Calo Check
  std::unique_ptr<CaloCheck> calo_check = std::make_unique<CaloCheck>();
  calo_check->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  calo_check->set_event_id(event_id);
  se->registerSubsystem(calo_check.release());

  std::unique_ptr<Fun4AllInputManager> In = std::make_unique<Fun4AllDstInputManager>("in");
  In->AddFile(fname);
  se->registerInputManager(In.release());

  se->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->run();
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

  if (args.size() < 2 || args.size() > 4)
  {
    std::cerr << "usage: " << args[0] << " <input_DST> [event_id] [dbtag]" << std::endl;
    std::cerr << "  input_DST: path to the input list file" << std::endl;
    std::cerr << "  event_id: (optional) Specific Event to Analyze (default: -1)" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: newcdbtag)" << std::endl;
    return 1;  // Indicate error
  }

  const std::string& input_dst= args[1];
  int event_id = 0;
  std::string dbtag = "newcdbtag";

  if (args.size() >= 3)
  {
    event_id = std::stoi(args[2]);
  }
  if (args.size() >= 4)
  {
    dbtag = args[3];
  }

  Fun4All_CaloCheck(input_dst, event_id, dbtag);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
# endif
