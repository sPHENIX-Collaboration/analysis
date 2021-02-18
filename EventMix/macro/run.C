#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include </direct/phenix+u/workarea/lebedev/sPHENIX_new/analysis/EventMix/install/include/eventmix/PairMaker.h>
#include </direct/phenix+u/workarea/lebedev/sPHENIX_new/analysis/EventMix/install/include/eventmix/sPHElectronPair.h>
#include </direct/phenix+u/workarea/lebedev/sPHENIX_new/analysis/EventMix/install/include/eventmix/sPHElectronPairv1.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libeventmix.so)
#endif

void run(const char *fname = "/sphenix/user/lebedev/mdc/sPHENIX_pythiaupsilons_10.root")
{
  gSystem->Load("libg4dst");
  gSystem->Load("libeventmix");

  Fun4AllServer *se = Fun4AllServer::instance();
  PairMaker *pmaker = new PairMaker("PairMaker","test.root");
  se->registerSubsystem(pmaker);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
  in->fileopen(fname);
  se->registerInputManager(in);
  //in->AddListFile("filelist.txt");

  Fun4AllOutputManager *outee = new Fun4AllDstOutputManager("outee","test.root");
  outee->AddNode("ElectronPairs");
  se->registerOutputManager(outee);
  outee->Print();

  se->run();

  outee->Print();

  se->End();
}
