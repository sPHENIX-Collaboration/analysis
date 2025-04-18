#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

//#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/install/include/trackpidassoc/TrackPidAssoc.h>
#include </direct/phenix+u/workarea/lebedev/test/analysis/install/include/electronid/ElectronID.h>
#include </direct/phenix+u/workarea/lebedev/test/analysis/install/include/eventmix/PairMaker.h>
//#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/install/include/eventmix/sPHElectronPair.h>
//#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/install/include/eventmix/sPHElectronPairv1.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libtrackpid.so)
R__LOAD_LIBRARY(libelectronid.so)
R__LOAD_LIBRARY(libeventmix.so)
#endif

void runall()
{
  gSystem->Load("libg4dst");
  gSystem->Load("libtrackpid");
  gSystem->Load("libelectronid");
  gSystem->Load("libeventmix");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  ElectronID* eid = new ElectronID("ElectronID","test_electronid.root");
  eid->setEMOPcutlimits(0.7,999.);
  eid->setHinOEMcutlimit(0.2);
  eid->setPtcutlimit(2.0,30.0);
  eid->setHOPcutlimit(0.3);
  se->registerSubsystem(eid);

  PairMaker *pmaker = new PairMaker("PairMaker","test_eventmix.root");
  pmaker->Verbosity(1);
  se->registerSubsystem(pmaker);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
  in->Verbosity(1);
  se->registerInputManager(in);
  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedupsilonsDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000006-00692.root");
  //in->AddFile("/sphenix/u/weihuma/RunOutput/embedDST_sHijing_upsilon_0_20fm_ElectronPid_DST.root");
  //in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04594.root");  
  //in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04595.root");
  //in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04596.root");
  //in->AddListFile("listmb2.txt");

  Fun4AllOutputManager *outee = new Fun4AllDstOutputManager("outee","/sphenix/user/lebedev/mdc/ElectronPid_PairMaker_DST.root");
  outee->Verbosity(1);
  outee->AddNode("ElectronPairs");
  se->registerOutputManager(outee);
  outee->Print();

  se->run();
  //se->run(10);

  se->End();
}

