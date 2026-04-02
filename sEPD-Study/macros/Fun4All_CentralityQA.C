// c++ includes --
#include <string>
#include <iostream>
#include <format>

// root includes --
#include <TSystem.h>
#include <TROOT.h>
#include <TF1.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllUtils.h>
#include <phool/recoConsts.h>

#include <mbd/MbdReco.h>
#include <mbd/MbdEvent.h>
#include <zdcinfo/ZdcReco.h>
#include <globalvertex/GlobalVertexReco.h>
#include <centrality/CentralityReco.h>
#include <calotrigger/MinimumBiasClassifier.h>
#include <calotrigger/TriggerRunInfoReco.h>

#include <sepdvalidation/CentralityQA.h>

R__LOAD_LIBRARY(libsEPDValidation.so)

void Fun4All_CentralityQA(const std::string& flist,
                         const std::string& output = "test.root",
                         int nEvents = 100,
                         const std::string& dbtag = "newcdbtag")
{
  std::cout << "########################" << std::endl;
  std::cout << "Run Parameters" << std::endl;
  std::cout << "input: " << flist << std::endl;
  std::cout << "output: " << output << std::endl;
  std::cout << "nEvents: " << nEvents << std::endl;
  std::cout << "dbtag: " << dbtag << std::endl;
  std::cout << "########################" << std::endl;

  Fun4AllServer *se = Fun4AllServer::instance();

  recoConsts *rc = recoConsts::instance();

  std::ifstream infile_stream;
  infile_stream.open(flist, std::ios_base::in);
  std::string filepath;
  getline(infile_stream, filepath);
  std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(filepath);
  long unsigned int runnumber = static_cast<long unsigned int>(runseg.first);
  infile_stream.close();

  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(1);

  FlagHandler* flag = new FlagHandler();
  se->registerSubsystem(flag);

  // MBD Reconstruction
  MbdReco* mbdreco = new MbdReco();
  se->registerSubsystem(mbdreco);

  // ZDC Reconstruction
  ZdcReco* zdcreco = new ZdcReco();
  zdcreco->set_zdc1_cut(0.0);
  zdcreco->set_zdc2_cut(0.0);
  se->registerSubsystem(zdcreco);

  // Vertex Reconstruction
  GlobalVertexReco* gvertexreco = new GlobalVertexReco();
  se->registerSubsystem(gvertexreco);

  // custom centrality calib
  std::string cent_calib_dir = "/direct/sphenix+u/anarde/Documents/sPHENIX/analysis-sEPD-Study/sEPD-Study/files/centrality/04-01-26";
  std::string cent_divs = std::format("{}/cdb_centrality_68144.root", cent_calib_dir);
  std::string cent_scale = std::format("{}/cdb_centrality_scale_68144.root", cent_calib_dir);
  std::string cent_vtx = std::format("{}/cdb_centrality_vertex_scale_68144.root", cent_calib_dir);

  // Minimum Bias Classifier
  MinimumBiasClassifier* mb = new MinimumBiasClassifier();
  mb->setOverwriteScale(cent_scale);
  mb->setOverwriteVtx(cent_vtx);
  se->registerSubsystem(mb);

  // Centrality Reco
  CentralityReco* cent = new CentralityReco();
  cent->setOverwriteDivs(cent_divs);
  cent->setOverwriteScale(cent_scale);
  cent->setOverwriteVtx(cent_vtx);
  se->registerSubsystem(cent);

  // Trigger Info Reco
  TriggerRunInfoReco* trig = new TriggerRunInfoReco();
  trig->Verbosity(1);
  se->registerSubsystem(trig);

  // Centrality QA
  CentralityQA* centrality_QA = new CentralityQA();
  centrality_QA->Verbosity(1);
  se->registerSubsystem(centrality_QA);

  Fun4AllInputManager* In = new Fun4AllDstInputManager("in");
  In->AddListFile(flist);
  se->registerInputManager(In);

  se->Verbosity(Fun4AllBase::VERBOSITY_QUIET);
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

# ifndef __CINT__
int main(int argc, const char* const argv[])
{
  const std::vector<std::string> args(argv, argv + argc);

  if (args.size() < 2 || args.size() > 5)
  {
    std::cerr << "usage: " << args[0] << " <input_DST> [output] [nEvents] [dbtag]" << std::endl;
    std::cerr << "  input_DST: path to the input DST file" << std::endl;
    std::cerr << "  output: (optional) path to the output file (default: 'test.root')" << std::endl;
    std::cerr << "  nEvents: (optional) number of events to process (default: 100)" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: newcdbtag)" << std::endl;
    return 1;  // Indicate error
  }

  size_t arg_ctr = 1;
  const std::string& input_dst = args[arg_ctr++];
  std::string output = (args.size() >= arg_ctr+1) ? args[arg_ctr++] : "test.root";
  int nEvents = (args.size() >= arg_ctr+1) ? std::stoi(args[arg_ctr++]) : 100;
  std::string dbtag = (args.size() >= arg_ctr+1) ? args[arg_ctr++] : "newcdbtag";

  Fun4All_CentralityQA(input_dst, output, nEvents, dbtag);

  std::cout << "======================================" << std::endl;
  std::cout << "done" << std::endl;
  return 0;
}
# endif
