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
#include <fun4all/Fun4AllBase.h>

#include <phool/recoConsts.h>

#include <sepdvalidation/sEPDValidation.h>

#include <CDBUtils.C>

R__LOAD_LIBRARY(libsEPDValidation.so)

void Fun4All_sEPD(const std::string &fname,
                  unsigned int runnumber,
                  const std::string &output = "test.root",
                  int nEvents = 100,
                  const std::string &dbtag = "newcdbtag",
                  bool condor_mode = false,
                  bool do_ep = true)
{
  std::cout << "########################" << std::endl;
  std::cout << "Run Parameters" << std::endl;
  std::cout << "input: " << fname << std::endl;
  std::cout << "Run: " << runnumber << std::endl;
  std::cout << "output: " << output << std::endl;
  std::cout << "nEvents: " << nEvents << std::endl;
  std::cout << "dbtag: " << dbtag << std::endl;
  std::cout << "Condor Mode: " << condor_mode << std::endl;
  std::cout << "Do Event Plane Reco: " << do_ep << std::endl;
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

  // // MBD Reconstruction
  std::unique_ptr<MbdReco> mbdreco = std::make_unique<MbdReco>();
  se->registerSubsystem(mbdreco.release());

  // sEPD Reconstruction--Calib Info
  std::unique_ptr<EpdReco> epdreco = std::make_unique<EpdReco>();
  se->registerSubsystem(epdreco.release());

  // ZDC Reconstruction--Calib Info
  std::unique_ptr<ZdcReco> zdcreco = std::make_unique<ZdcReco>();
  zdcreco->set_zdc1_cut(0.0);
  zdcreco->set_zdc2_cut(0.0);
  se->registerSubsystem(zdcreco.release());

  // Official vertex storage
  std::unique_ptr<GlobalVertexReco> gvertex = std::make_unique<GlobalVertexReco>();
  gvertex->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(gvertex.release());

  setGlobalTag(dbtag);

  uint64_t default_centrality_run = 54912;
  std::string cdb_centrality              = getCalibration("Centrality", runnumber);
  std::string cdb_centrality_scale        = getCalibration("CentralityScale", runnumber);
  std::string cdb_centrality_vertex_scale = getCalibration("CentralityVertexScale", runnumber);

  bool useReferenceCDB = cdb_centrality.starts_with("DataBaseException")
                      || cdb_centrality_scale.starts_with("DataBaseException")
                      || cdb_centrality_vertex_scale.starts_with("DataBaseException");

  // Check if centrality cdb files exists for the current runs
  // if not then use the cdb files from the reference run
  if(useReferenceCDB)
  {
    std::cout << "Defaulting centrality cdb from Run " << default_centrality_run << std::endl;

    cdb_centrality              = getCalibration("Centrality", default_centrality_run);
    cdb_centrality_scale        = getCalibration("CentralityScale", default_centrality_run);
    cdb_centrality_vertex_scale = getCalibration("CentralityVertexScale", default_centrality_run);
  }

  // Minimum Bias Classifier
  std::unique_ptr<MinimumBiasClassifier> mb = std::make_unique<MinimumBiasClassifier>();
  mb->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  mb->set_mbd_total_charge_cut(2100);
  if(useReferenceCDB)
  {
    mb->setOverwriteScale(cdb_centrality_scale);
    mb->setOverwriteVtx(cdb_centrality_vertex_scale);
  }
  se->registerSubsystem(mb.release());

  // Centrality
  std::unique_ptr<CentralityReco> cent = std::make_unique<CentralityReco>();
  cent->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  if(useReferenceCDB)
  {
    cent->setOverwriteDivs(cdb_centrality);
    cent->setOverwriteScale(cdb_centrality_scale);
    cent->setOverwriteVtx(cdb_centrality_vertex_scale);
  }
  se->registerSubsystem(cent.release());

  // Event Plane
  if(do_ep)
  {
    std::unique_ptr<EventPlaneReco> epreco = std::make_unique<EventPlaneReco>();
    epreco->set_sepd_epreco(true);
    epreco->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
    se->registerSubsystem(epreco.release());
  }

  // sEPD QA
  std::unique_ptr<sEPDValidation> sepd_validation = std::make_unique<sEPDValidation>();
  sepd_validation->set_filename(output);
  sepd_validation->set_condor_mode(condor_mode);
  sepd_validation->set_do_ep(do_ep);
  sepd_validation->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
  se->registerSubsystem(sepd_validation.release());

  std::unique_ptr<Fun4AllInputManager> In = std::make_unique<Fun4AllDstInputManager>("in");
  In->AddListFile(fname);
  se->registerInputManager(In.release());

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
int main(int argc, const char* const argv[])
{
  const std::vector<std::string> args(argv, argv + argc);

  if (args.size() < 3 || args.size() > 8)
  {
    std::cerr << "usage: " << args[0] << " <input_DST_list> <runnumber> [output] [nEvents] [dbtag] [condor_mode] [do_ep]" << std::endl;
    std::cerr << "  input_DST: path to the input list file" << std::endl;
    std::cerr << "  runnumber: Run" << std::endl;
    std::cerr << "  output: (optional) path to the output file (default: 'test.root')" << std::endl;
    std::cerr << "  nEvents: (optional) number of events to process (default: 100)" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: prodA_2024)" << std::endl;
    std::cerr << "  Condor Mode: set condor mode for efficient output file." << std::endl;
    std::cerr << "  Do Event Plane: Do official Event Plane reconstruction." << std::endl;
    return 1;  // Indicate error
  }

  const std::string& input_dst= args[1];
  unsigned int runnumber = static_cast<unsigned int>(std::stoul(args[2]));
  std::string output = "test.root";
  int nEvents = 100;
  std::string dbtag = "newcdbtag";
  bool condor_mode = false;
  bool do_ep = true;

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
    dbtag = args[5];
  }
  if (args.size() >= 7)
  {
    condor_mode = std::stoi(args[6]);
  }
  if (args.size() >= 8)
  {
    do_ep = std::stoi(args[7]);
  }

  Fun4All_sEPD(input_dst, runnumber, output, nEvents, dbtag, condor_mode, do_ep);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
# endif
