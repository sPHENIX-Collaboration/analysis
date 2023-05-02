
#ifndef MACRO_FUN4ALL_READDST_C
#define MACRO_FUN4ALL_READDST_C

#include <tpcrawtottree/TPCRawDataTree.h>

#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libTPCRawDataTree.so)

int Fun4All_TPC_UnpackPRDF(const int nEvents = 50,
                           const string &inputFile = "/sphenix/user/jinhuang/TPC/commissioning/TPC_ebdc14_junk-00010050-0000.prdf"  //
)
{
  //---------------
  // Fun4All server
  //---------------
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  TPCRawDataTree *r2tree = new TPCRawDataTree(inputFile + "_TPCRawDataTree.root");
  se->registerSubsystem(r2tree);

  Fun4AllPrdfInputManager *in1 = new Fun4AllPrdfInputManager("PRDF1");
  in1->AddFile(inputFile);
  se->registerInputManager(in1);

  se->run(nEvents);

  se->End();

  delete se;
  std::cout << "All done processing" << std::endl;
  gSystem->Exit(0);
  return 0;
}
#endif
