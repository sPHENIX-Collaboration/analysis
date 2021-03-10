#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include </sphenix/u/weihuma/install/include/eventmix/PairMaker.h>
#include </sphenix/u/weihuma/install/include/trackpidassoc/ElectronPid.h>

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

  ElectronPid* eid = new ElectronPid("ElectronPid","/sphenix/u/weihuma/RunOutput/EVENTMIX/Upsilon_electrons_cutting_ntuple.root");
  eid->setEMOPcutlimits(0.7,1.5);
  eid->setHinOEMcutlimit(0.2);
  eid->setPtcutlimit(2.0,30.0);
  eid->setHOPcutlimit(0.3);
  se->registerSubsystem(eid);

  PairMaker *pmaker = new PairMaker("PairMaker","/sphenix/u/weihuma/RunOutput/EVENTMIX/Upsilon_electrons_cutting_dummy.root");
  se->registerSubsystem(pmaker);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
  in->Verbosity(1);
  se->registerInputManager(in);
  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04594.root");  
  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04595.root");
  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04596.root");
  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04597.root");
  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04598.root");
  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04599.root");


  //in->AddListFile("listmb2.txt");

  Fun4AllOutputManager *outee = new Fun4AllDstOutputManager("outee","/sphenix/u/weihuma/RunOutput/EVENTMIX/Upsilon_electrons_cutting_0_20fm.root");
  outee->Verbosity(1);
  outee->AddNode("ElectronPairs");
  se->registerOutputManager(outee);
  outee->Print();

  se->run();

  outee->Print();

  se->End();
}

