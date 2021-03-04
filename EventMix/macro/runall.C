#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/test/analysis/EventMix/install/include/eventmix/PairMaker.h>
#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/test/analysis/EventMix/install/include/trackpidassoc/ElectronPid.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libeventmix.so)
R__LOAD_LIBRARY(libtrackpid.so)
#endif

void runall()
{
  gSystem->Load("libg4dst");
  gSystem->Load("libeventmix");
  gSystem->Load("libtrackpid");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  ElectronPid* eid = new ElectronPid("ElectronPid");
  se->registerSubsystem(eid);

  PairMaker *pmaker = new PairMaker("PairMaker","dummy.root");
  se->registerSubsystem(pmaker);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
  in->Verbosity(1);
  se->registerInputManager(in);
  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-02990.root");
  //in->AddListFile("listmb2.txt");

  Fun4AllOutputManager *outee = new Fun4AllDstOutputManager("outee","test.root");
  outee->Verbosity(1);
  outee->AddNode("ElectronPairs");
  se->registerOutputManager(outee);
  outee->Print();

  se->run(5);

  outee->Print();

  se->End();
}

