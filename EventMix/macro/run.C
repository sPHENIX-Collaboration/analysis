#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include </direct/phenix+u/workarea/lebedev/test/analysis/install/include/eventmix/PairMaker.h>
#include </direct/phenix+u/workarea/lebedev/test/analysis/install/include/eventmix/sPHElectronPair.h>
#include </direct/phenix+u/workarea/lebedev/test/analysis/install/include/eventmix/sPHElectronPairv1.h>

//#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/test/analysis/EventMix/install/include/eventmix/PairMaker.h>
//#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/test/analysis/EventMix/install/include/eventmix/sPHElectronPair.h>
//#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/test/analysis/EventMix/install/include/eventmix/sPHElectronPairv1.h>

//#include </sphenix/u/weihuma/install/include/eventmix/PairMaker.h>
//#include </sphenix/u/weihuma/install/include/eventmix/sPHElectronPair.h>
//#include </sphenix/u/weihuma/install/include/eventmix/sPHElectronPairv1.h>

//#include </sphenix/u/weihuma/install/include/trackpidassoc/ElectronPid.h>
//#include </sphenix/u/weihuma/install/include/trackpidassoc/TrackPidAssoc.h>


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libeventmix.so)
R__LOAD_LIBRARY(libtrackpid.so)

#endif

//void run()
//void run(const char *fname = "/sphenix/user/lebedev/mdc/pythiaupsilons/sPHENIX_pythiaupsilons_10.root")
//void run(const char *fname = "/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-02990.root")
void run(
  const char *fname = "/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedupsilonsDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000006-00692.root"
)

{
  gSystem->Load("libg4dst");
  gSystem->Load("libeventmix");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  PairMaker *pmaker = new PairMaker("PairMaker","dummy.root");
  pmaker->Verbosity(1);
  se->registerSubsystem(pmaker);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
  in->Verbosity(1);
  se->registerInputManager(in);
  in->AddFile(fname);
  //in->AddListFile("listmb2.txt");

  Fun4AllOutputManager *outee = new Fun4AllDstOutputManager("outee","/sphenix/user/lebedev/mdc/test.root");
  outee->Verbosity(1);
  outee->AddNode("ElectronPairs");
  se->registerOutputManager(outee);
  outee->Print();

  se->run();

 //se->run(10);
  se->run();
  outee->Print();

  se->End();
}
