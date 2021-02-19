#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include </sphenix/u/weihuma/install/include/trackpidassoc/ElectronPid.h>
#include </sphenix/u/weihuma/install/include/trackpidassoc/TrackPidAssoc.h>
//#include </direct/phenix+u/workarea/lebedev/sPHENIX_new/analysis/EventMix/install/include/eventmix/sPHElectronPair.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libtrackpid.so)
//R__LOAD_LIBRARY(libeventmix.so)
#endif

void run(const char *fname = "/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_12fm_50kHz_bkg_0_12fm-0000000001-01998.root")
{
  gSystem->Load("libg4dst");
  gSystem->Load("libtrackpid");
  //  gSystem->Load("libeventmix");

  Fun4AllServer *se = Fun4AllServer::instance();
  ElectronPid *ePid = new ElectronPid("ElectronPid");
  //  PairMaker *pmaker = new PairMaker("PairMaker","test.root");
  se->registerSubsystem(ePid);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
  in->fileopen(fname);
  se->registerInputManager(in);
  //in->AddListFile("filelist.txt");

 // Fun4AllServer *se = Fun4AllServer::instance();
 // ElectronPid *ePid = new ElectronPid("ElectronPid");
  //  PairMaker *pmaker = new PairMaker("PairMaker","test.root");
   //se->registerSubsystem(ePid);

  Fun4AllOutputManager *outePid = new Fun4AllDstOutputManager("outePid","test6.root");
  outePid->AddNode("TrackPidAssoc");
  //  outePid->AddNode("ElectronPairs");
  se->registerOutputManager(outePid);
  outePid->Print();

  se->run();
  
  ePid->Print();
  outePid->Print();

  se->End();

}
