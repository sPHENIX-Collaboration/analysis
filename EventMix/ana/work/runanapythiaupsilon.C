#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/install/include/sphanalysis/sPHAnalysis.h>
#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/install/include/eventmix/sPHElectronPair.h>
#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/install/include/eventmix/sPHElectronPairv1.h>
#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/install/include/eventmix/sPHElectronPairContainer.h>
#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/install/include/eventmix/sPHElectronPairContainerv1.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libeventmix.so)
R__LOAD_LIBRARY(libsphanalysis.so)
#endif

void runanapythiaupsilon()
{
  gSystem->Load("libg4dst");
  gSystem->Load("libeventmix");
  gSystem->Load("libsphanalysis");

  char outfilename[99];
  char infilename[99];
  //sprintf(outfilename,"/sphenix/user/lebedev/mdc/embedmb_withpileup_bb.root");
  sprintf(outfilename,"test.root");
  cout << "output file: " << outfilename << endl;

  Fun4AllServer *se = Fun4AllServer::instance();
  sPHAnalysis *ana = new sPHAnalysis("sPHAnalysis",outfilename);
  ana->set_whattodo(2);
  se->registerSubsystem(ana);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
  //in->fileopen(fname);
  se->registerInputManager(in);
  //in->AddListFile("pions3.txt");
  //in->AddListFile("kaons.txt");
  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/pythiaupsilonDST_pythia6-0000000000-00101.root");

//  for(int i=0; i<10; i++) {
//    sprintf(infilename,"/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-0%d.root",kkk+i);
//    cout << "in:  " << infilename << endl;
//    in->AddFile(infilename);
//  }

  se->run();
  se->End();
}

