#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include </gpfs/mnt/gpfs02/sphenix/user/lebedev/mdc/analysis/EventMix/ana_calo/install/include/sphanalysis_calo/sPHAnalysis_calo.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libsphanalysis_calo.so)

void run()
{
  Fun4AllServer *se = Fun4AllServer::instance();
  sPHAnalysis_calo *ana = new sPHAnalysis_calo("sPHAnalysis_calo","test.root");
  se->registerSubsystem(ana);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
  se->registerInputManager(in);
  //in->AddListFile("upsilons1.txt");
  in->AddListFile("singleupsilons1.txt");
  //in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedupsilonsDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000006-00690.root");

  se->run();
  se->End();
}

