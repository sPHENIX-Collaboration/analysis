
#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <trackreco/PHGenFitTrackProjection.h>

#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/FilterEventsUpsilon/install/include/filtereventsupsilon/FilterEventsUpsilon.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfiltereventsupsilon.so)
#endif

void run()
{

  gSystem->Load("libg4dst");
  gSystem->Load("libeventmix");

  char outfilename[99];
  char infilename1[99];
  char infilename2[99];
  sprintf(outfilename,"test.root");
  cout << "out: " << outfilename << endl;

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  FilterEventsUpsilon *filter = new FilterEventsUpsilon("FilterEventsUpsilon");
  se->registerSubsystem(filter);

  Fun4AllInputManager *in1 = new Fun4AllDstInputManager("in1");
  se->registerInputManager(in1);
  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("in2");
  se->registerInputManager(in2);

  for(int i=0; i<10; i++) {
    sprintf(infilename1,"DST_TRACKS_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-0%d.root",kkk+i);
    cout << "in1:  " << infilename1 << endl;
    in1->AddFile(infilename1);
    sprintf(infilename2,"DST_CALO_CLUSTER_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-0%d.root",kkk+i);
    cout << "in2:  " << infilename2 << endl;
    in2->AddFile(infilename2);
  }

  Fun4AllOutputManager *outee = new Fun4AllDstOutputManager("outee",oufilename);
  outee->Verbosity(1);
  outee->AddNode("SvtxTrackSeedContainer");
  outee->AddNode("SvtxVertexMap");
  outee->AddNode("SvtxTrackMap");
  outee->AddNode("CLUSTER_CEMC");
  se->registerOutputManager(outee);
  outee->Print();

  se->run();
  se->End();
}



