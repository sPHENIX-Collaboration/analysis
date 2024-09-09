
#ifndef MACRO_FUN4ALL_READDST_C
#define MACRO_FUN4ALL_READDST_C

#include <tpcrawtottree/TpcTimeFrameChecker.h>

#include <fun4all/Fun4AllServer.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <QA.C>

#include <phool/recoConsts.h>
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libTPCRawDataTree.so)

int Fun4All_TpcTimeFrameChecker(const int nEvents = 1000,
                                const string &inputFile = "/direct/sphenix+lustre01/sphnxpro/physics/tpc/physics/TPC_ebdc00_physics-00052482-0000.evt"  //
)
{
  //---------------
  // Fun4All server
  //---------------
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  for (int i = 4000; i <= 4001; i++)
  {
    TpcTimeFrameChecker *checker = new TpcTimeFrameChecker(i);
    // checker->Verbosity(1);
    checker->setDebugTTreeFile("Fun4All_TpcTimeFrameChecker_qa" + to_string(i) + ".root");
    se->registerSubsystem(checker);
  }

  Fun4AllPrdfInputManager *in1 = new Fun4AllPrdfInputManager("PRDF1");
  in1->AddFile(inputFile);
  se->registerInputManager(in1);

  gSystem->ListLibraries();
  se->run(nEvents);

  se->End();
  se->PrintTimer();

  QA_Output("Fun4All_TpcTimeFrameChecker_qa.root");

  delete se;
  std::cout << "All done processing" << std::endl;
  gSystem->Exit(0);
  return 0;
}

#endif
