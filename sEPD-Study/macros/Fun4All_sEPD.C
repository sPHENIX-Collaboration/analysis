// c++ includes --
#include <string>
#include <iostream>
#include <memory>
#include <filesystem>
#include <vector>

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
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllBase.h>

#include <phool/recoConsts.h>

#include <sepdvalidation/sEPDValidation.h>

R__LOAD_LIBRARY(libsEPDValidation.so)

void Fun4All_sEPD(const std::string &fname,
                  const std::string &output = "test.root",
                  int nEvents = 100,
                  const std::string &dbtag = "newcdbtag")
{
  std::cout << "########################" << std::endl;
  std::cout << "Run Parameters" << std::endl;
  std::cout << "input: " << fname << std::endl;
  std::cout << "output: " << output << std::endl;
  std::cout << "nEvents: " << nEvents << std::endl;
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

  std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(fname);
  UInt_t runnumber = static_cast<UInt_t>(runseg.first);

  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(Fun4AllBase::VERBOSITY_SOME);

  std::unique_ptr<FlagHandler> flag = std::make_unique<FlagHandler>();
  se->registerSubsystem(flag.get());

  // // MBD Reconstruction
  std::unique_ptr<MbdReco> mbdreco = std::make_unique<MbdReco>();
  se->registerSubsystem(mbdreco.get());

  // sEPD Reconstruction--Calib Info
  std::unique_ptr<EpdReco> epdreco = std::make_unique<EpdReco>();
  se->registerSubsystem(epdreco.get());

  // ZDC Reconstruction--Calib Info
  std::unique_ptr<ZdcReco> zdcreco = std::make_unique<ZdcReco>();
  zdcreco->set_zdc1_cut(0.0);
  zdcreco->set_zdc2_cut(0.0);
  se->registerSubsystem(zdcreco.get());

  // Official vertex storage
  std::unique_ptr<GlobalVertexReco> gvertex = std::make_unique<GlobalVertexReco>();
  gvertex->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(gvertex.get());

  // Minimum Bias Classifier
  std::unique_ptr<MinimumBiasClassifier> mb = std::make_unique<MinimumBiasClassifier>();
  mb->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(mb.get());

  // Centrality
  std::unique_ptr<CentralityReco> cent = std::make_unique<CentralityReco>();
  cent->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(cent.get());

  // Event Plane
  std::unique_ptr<EventPlaneReco> epreco = std::make_unique<EventPlaneReco>();
  epreco->set_sepd_epreco(true);
  epreco->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(epreco.get());

  // sEPD QA
  std::unique_ptr<sEPDValidation> sepd_validation = std::make_unique<sEPDValidation>();
  sepd_validation->set_filename(output);
  sepd_validation->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(sepd_validation.get());

  std::unique_ptr<Fun4AllInputManager> In = std::make_unique<Fun4AllDstInputManager>("in");
  In->AddFile(fname);
  se->registerInputManager(In.get());

  se->run(nEvents);
  se->End();

  CDBInterface::instance()->Print();  // print used DB files
  se->PrintTimer();
  delete se;
  std::cout << "All done!" << std::endl;
  gSystem->Exit(0);
  std::quick_exit(0);
}


# ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[])
{
  const std::vector<std::string> args(argv, argv + argc);

  if (args.size() < 2 || args.size() > 5)
  {
    std::cerr << "usage: " << args[0] << " <input_DST> [output] [nEvents] [dbtag]" << std::endl;
    std::cerr << "  input_DST: path to the input file" << std::endl;
    std::cerr << "  output_directory: (optional) path to the output file (default: 'test.root')" << std::endl;
    std::cerr << "  nEvents: (optional) number of events to process (default: 100)" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: prodA_2024)" << std::endl;
    return 1;  // Indicate error
  }

  const std::string& input_dst= args[1];
  std::string output = "test.root";
  Int_t nEvents = 100;
  std::string dbtag = "newcdbtag";

  if (args.size() >= 3)
  {
    output = args[2];
  }
  if (args.size() >= 4)
  {
    nEvents = std::stoi(args[3]);
  }
  if (args.size() >= 5)
  {
    dbtag = args[4];
  }

  Fun4All_sEPD(input_dst, output, nEvents, dbtag);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
# endif
