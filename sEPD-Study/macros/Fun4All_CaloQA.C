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

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllUtils.h>

#include <phool/recoConsts.h>

#include <sepdvalidation/CaloQA.h>

#include "Calo_Calib.C"

R__LOAD_LIBRARY(libsEPDValidation.so)

void Fun4All_CaloQA(const std::string &flist,
                    const std::string &output = "test.root",
                    int nEvents = 100,
                    const std::string &dbtag = "newcdbtag")
{
  std::cout << "########################" << std::endl;
  std::cout << "Run Parameters" << std::endl;
  std::cout << "input: " << flist << std::endl;
  std::cout << "output: " << output << std::endl;
  std::cout << "nEvents: " << nEvents << std::endl;
  std::cout << "dbtag: " << dbtag << std::endl;
  std::cout << "########################" << std::endl;

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(0);

  recoConsts *rc = recoConsts::instance();

  std::ifstream infile_stream;
  infile_stream.open(flist, std::ios_base::in);
  std::string filepath;
  getline(infile_stream, filepath);
  std::pair<int, int> runseg = Fun4AllUtils::GetRunSegment(filepath);
  unsigned int runnumber = static_cast<unsigned int>(runseg.first);
  infile_stream.close();

  // conditions DB flags and timestamp
  rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(1);

  std::unique_ptr<FlagHandler> flag = std::make_unique<FlagHandler>();
  se->registerSubsystem(flag.release());

  // MBD Reconstruction
  std::unique_ptr<MbdReco> mbdreco = std::make_unique<MbdReco>();
  se->registerSubsystem(mbdreco.release());

  // ZDC Reconstruction--Calib Info
  std::unique_ptr<ZdcReco> zdcreco = std::make_unique<ZdcReco>();
  zdcreco->set_zdc1_cut(0.0);
  zdcreco->set_zdc2_cut(0.0);
  se->registerSubsystem(zdcreco.release());

  // Official vertex storage
  std::unique_ptr<GlobalVertexReco> gvertex = std::make_unique<GlobalVertexReco>();
  se->registerSubsystem(gvertex.release());

  // Minimum Bias Classifier
  std::unique_ptr<MinimumBiasClassifier> mb = std::make_unique<MinimumBiasClassifier>();
  mb->set_mbd_total_charge_cut(2100);
  se->registerSubsystem(mb.release());

  // Centrality
  std::unique_ptr<CentralityReco> cent = std::make_unique<CentralityReco>();
  se->registerSubsystem(cent.release());

  // Calibrate Towers
  Process_Calo_Calib();

  // Calo QA
  std::unique_ptr<CaloQA> calo_qa = std::make_unique<CaloQA>();
  calo_qa->Verbosity(1);
  se->registerSubsystem(calo_qa.release());

  std::unique_ptr<Fun4AllInputManager> In = std::make_unique<Fun4AllDstInputManager>("in");
  In->AddListFile(flist);
  se->registerInputManager(In.release());

  se->run(nEvents);
  se->End();

  se->dumpHistos(output);

  CDBInterface::instance()->Print();  // print used DB files
  se->PrintTimer();
  delete se;
  std::cout << "All done!" << std::endl;
  gSystem->Exit(0);
}

# ifndef __CINT__
int main(int argc, const char* const argv[])
{
  const std::vector<std::string> args(argv, argv + argc);

  if (args.size() < 2 || args.size() > 5)
  {
    std::cerr << "usage: " << args[0] << " <input_DST_list> [output] [nEvents] [dbtag]" << std::endl;
    std::cerr << "  input_DST: path to the input list file" << std::endl;
    std::cerr << "  output: (optional) path to the output file (default: 'test.root')" << std::endl;
    std::cerr << "  nEvents: (optional) number of events to process (default: 100)" << std::endl;
    std::cerr << "  dbtag: (optional) database tag (default: newcdbtag)" << std::endl;
    return 1;  // Indicate error
  }

  const std::string& input_dst= args[1];
  std::string output = "test.root";
  int nEvents = 100;
  std::string dbtag = "newcdbtag";

  unsigned int ctr = 2;

  if (args.size() >= ctr+1)
  {
    output = args[ctr++];
  }
  if (args.size() >= ctr+1)
  {
    nEvents = std::stoi(args[ctr++]);
  }
  if (args.size() >= ctr+1)
  {
    dbtag = args[ctr++];
  }

  Fun4All_CaloQA(input_dst, output, nEvents, dbtag);

  std::cout << "======================================" << std::endl;
  std::cout << "All done!" << std::endl;
  return 0;
}
# endif
