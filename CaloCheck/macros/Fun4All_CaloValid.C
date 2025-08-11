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

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllBase.h>

#include <phool/recoConsts.h>

#include <qautils/QAHistManagerDef.h>

#include <calovalid/CaloValid.h>

#include <Calo_Calib.C>


R__LOAD_LIBRARY(libcalovalid.so)

void Fun4All_CaloValid(const std::string &fname,
                       unsigned int runnumber,
                       const std::string &output = "test.root",
                       int nEvents = 0,
                       float emcMinClusE1 = 1.3F,
                       float emcMinClusE2 = 0.7F,
                       const std::string &dbtag = "newcdbtag")
{
  std::cout << "########################" << std::endl;
  std::cout << "Run Parameters" << std::endl;
  std::cout << std::format("input: {}", fname) << std::endl;
  std::cout << std::format("Run: {}", runnumber) << std::endl;
  std::cout << std::format("output: {}", output) << std::endl;
  std::cout << std::format("nEvents: {}", nEvents) << std::endl;
  std::cout << std::format("emc min cluster E1: {}", emcMinClusE1) << std::endl;
  std::cout << std::format("emc min cluster E2: {}", emcMinClusE2) << std::endl;
  std::cout << std::format("dbtag: {}", dbtag) << std::endl;
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

  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(Fun4AllBase::VERBOSITY_SOME);

  std::unique_ptr<FlagHandler> flag = std::make_unique<FlagHandler>();
  se->registerSubsystem(flag.release());

  // MBD Reconstruction
  std::unique_ptr<MbdReco> mbdreco = std::make_unique<MbdReco>();
  se->registerSubsystem(mbdreco.release());

  // Official vertex storage
  std::unique_ptr<GlobalVertexReco> gvertex = std::make_unique<GlobalVertexReco>();
  gvertex->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(gvertex.release());

  Process_Calo_Calib();

  // Validation
  std::unique_ptr<CaloValid> ca = std::make_unique<CaloValid>("CaloValid");
  ca->set_timing_cut_width(200);
  ca->set_emcMinClusE1(emcMinClusE1);
  ca->set_emcMinClusE2(emcMinClusE2);
  se->registerSubsystem(ca.release());

  std::unique_ptr<Fun4AllInputManager> In = std::make_unique<Fun4AllDstInputManager>("in");
  In->AddListFile(fname);
  se->registerInputManager(In.release());

  se->run(nEvents);
  se->End();

  QAHistManagerDef::saveQARootFile(output);

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

  if (args.size() < 3 || args.size() > 8)
  {
    std::cerr << "usage: " << args[0] << " <input_DST_list> <runnumber> [output] [nEvents] [emcMinClusE1] [emcMinClusE2] [dbtag]" << std::endl;
    std::cerr << "  input_DST: path to the input list file" << std::endl;
    std::cerr << "  runnumber: Run" << std::endl;
    std::cerr << "  output: (optional) path to the output file (default: 'test.root')" << std::endl;
    std::cerr << "  nEvents: (optional) number of events to process (default: 100)" << std::endl;
    std::cerr << "  emcMinClusE1: (optional) min energy of cluster 1 (default: 1.3 GeV)" << std::endl;
    std::cerr << "  emcMinClusE2: (optional) min energy of cluster 2 (default: 0.7 GeV)" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: prodA_2024)" << std::endl;
    return 1;  // Indicate error
  }

  const std::string& input_dst= args[1];
  unsigned int runnumber = static_cast<unsigned int>(std::stoul(args[2]));
  std::string output = "test.root";
  int nEvents = 0;
  float emcMinClusE1 = 1.3F;
  float emcMinClusE2 = 0.7F;
  std::string dbtag = "newcdbtag";

  if (args.size() >= 4)
  {
    output = args[3];
  }
  if (args.size() >= 5)
  {
    nEvents = std::stoi(args[4]);
  }
  if (args.size() >= 6)
  {
    emcMinClusE1 = std::stof(args[5]);
  }
  if (args.size() >= 7)
  {
    emcMinClusE2 = std::stof(args[6]);
  }
  if (args.size() >= 8)
  {
    dbtag = args[7];
  }

  Fun4All_CaloValid(input_dst, runnumber, output, nEvents, emcMinClusE1, emcMinClusE2, dbtag);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
# endif
