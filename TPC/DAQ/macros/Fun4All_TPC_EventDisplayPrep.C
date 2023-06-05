
#ifndef MACRO_FUN4ALL_READDST_C
#define MACRO_FUN4ALL_READDST_C

#include <tpcrawtottree/TPCRawDataTree.h>

#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libTPCRawDataTree.so)

int Fun4All_TPC_EventDisplayPrep(const int nEvents = 10,
			   const string &outDir = "/sphenix/user/rosstom/test/testFiles/",
                           const string &inputFile = "/sphenix/lustre01/sphnxpro/rawdata/commissioning/TPC_ebdc00_pedestal-00010619-0000.prdf"  //
)
{
  //---------------
  // Fun4All server
  //---------------
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  string fileName = inputFile;
  size_t pos = fileName.find("TPC_ebdc");
  fileName.erase(fileName.begin(),fileName.begin()+pos);
  
  TPCRawDataTree *r2tree = new TPCRawDataTree(outDir + fileName + "_TPCRawDataTree.root");

  // add all possible TPC packet that we need to analyze
  for (int packet = 4000; packet<=4230; packet+=10)
  {
    r2tree->AddPacket(packet);
    r2tree->AddPacket(packet+1);
  }

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
