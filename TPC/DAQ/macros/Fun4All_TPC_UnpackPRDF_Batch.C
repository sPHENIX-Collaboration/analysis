
#ifndef MACRO_FUN4ALL_READDST_C
#define MACRO_FUN4ALL_READDST_C

#include <tpcrawtottree/TPCRawDataTree.h>

#include <fun4all/Fun4AllServer.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>

#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libTPCRawDataTree.so)

int Fun4All_TPC_UnpackPRDF_Batch(
    const int nEvents = 400,
    const int sector = 11,
    const int run = 11127,
    const string &mode = "beam")
{
  //---------------
  // Fun4All server
  //---------------
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  const string inputFile =
      Form("/sphenix/lustre01/sphnxpro/commissioning/tpc/%s/TPC_ebdc%02d_%s-000%05d-0000.prdf",
           mode.c_str(), sector, mode.c_str(), run);

  string outFile = Form("/phenix/u/jinhuang/links/sPHENIX_work/TPC/commissioning/TPC_ebdc%02d_%s-000%05d-0000.prdf.TPCRawDataTree.root",
                        sector, mode.c_str(), run);

  string fileName = inputFile;
  size_t pos = fileName.find("TPC_ebdc");
  fileName.erase(fileName.begin(), fileName.begin() + pos);

  TPCRawDataTree *r2tree = new TPCRawDataTree(outFile);

  // add all possible TPC packet that we need to analyze
  for (int packet = 4000; packet <= 4230; packet += 10)
  {
    r2tree->AddPacket(packet);
    r2tree->AddPacket(packet + 1);
  }

  se->registerSubsystem(r2tree);

  Fun4AllPrdfInputManager *in1 = new Fun4AllPrdfInputManager("PRDF1");
  in1->fileopen(inputFile);
  se->registerInputManager(in1);

  // se->skip(8622); //////////////////////////////////////////////////
  se->run(nEvents);

  se->End();

  delete se;
  std::cout << "All done processing" << std::endl;
  gSystem->Exit(0);
  return 0;
}
#endif

