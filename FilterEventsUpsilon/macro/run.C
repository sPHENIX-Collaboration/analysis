
#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/FilterEventsUpsilon/install/include/filtereventsupsilon/FilterEventsUpsilon.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfiltereventsupsilon.so)
#endif

void run(int kkk=0)
{

//  gSystem->Load("libg4dst");
//  gSystem->Load("libfiltereventsupsilon");

  char outfilename[99];
  char infilename1[99];
  char infilename2[99];
  sprintf(outfilename,"test.root");
  cout << "output file name: " << outfilename << endl;

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  Fun4AllInputManager *in1 = new Fun4AllDstInputManager("in1");
  se->registerInputManager(in1);
//  Fun4AllInputManager *in2 = new Fun4AllDstInputManager("in2");
//  se->registerInputManager(in2);
//  Fun4AllInputManager *in3 = new Fun4AllDstInputManager("in3");
//  se->registerInputManager(in3);
//  Fun4AllInputManager *in4 = new Fun4AllDstInputManager("in4");
//  se->registerInputManager(in4);

  in1->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embedpion/embedupsilonsDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-00110.root");
  in1->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embedpion/embedupsilonsDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-00111.root");
  in1->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embedpion/embedupsilonsDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-00112.root");
  in1->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embedpion/embedupsilonsDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-00113.root");
  in1->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embedpion/embedupsilonsDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-00114.root");

//  in1->AddFile("DST_TRACKS_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-00090.root");
//  in2->AddFile("DST_TRACKSEEDS_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-00090.root");
//  in3->AddFile("DST_TRKR_CLUSTER_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-00090.root");
//  in4->AddFile("DST_CALO_CLUSTER_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-00090.root");

/*
  for(int i=0; i<10; i++) {
    sprintf(infilename1,"DST_TRACKS_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-%05d.root",kkk+i);
    cout << "in1:  " << infilename1 << endl;
    in1->AddFile(infilename1);
    sprintf(infilename2,"DST_CALO_CLUSTER_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000060-%05d.root",kkk+i);
    cout << "in2:  " << infilename2 << endl;
    in2->AddFile(infilename2);
  }
*/

  FilterEventsUpsilon *filter = new FilterEventsUpsilon("FilterEventsUpsilon");
  se->registerSubsystem(filter);

  Fun4AllOutputManager *outee = new Fun4AllDstOutputManager("outee",outfilename);
  outee->Verbosity(1);
  outee->AddNode("SvtxTrackMap_ee");
  outee->AddNode("TRKR_CLUSTER");
  outee->AddNode("SiliconTrackSeedContainer");
  outee->AddNode("TpcTrackSeedContainer");
  outee->AddNode("SvtxTrackSeedContainer");
  outee->AddNode("CLUSTER_CEMC");
  outee->AddNode("SvtxVertexMap");
  outee->AddNode("GlobalVertexMap");
  se->registerOutputManager(outee);
  outee->Print();

  se->run(10);
  se->End();
cout << "all done." << endl;
}



