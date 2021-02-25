#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <GlobalVariables.C>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include <anatutorial/AnaTutorial.h>
#include </sphenix/u/weihuma/install/include/trackpidassoc/ElectronPid.h>
#include </sphenix/u/weihuma/install/include/trackpidassoc/TrackPidAssoc.h>
//#include </direct/phenix+u/workarea/lebedev/sPHENIX_new/analysis/EventMix/install/include/eventmix/sPHElectronPair.h>

R__LOAD_LIBRARY(libanatutorial.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libtrackpid.so)
//R__LOAD_LIBRARY(libeventmix.so)
#endif

void run(
  const char *inputFile = "/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_12fm_50kHz_bkg_0_12fm-0000000001-01998.root",
  const string &outputroot = "embedDST_sHijing_upsilon_0_12fm_EOP_0.7_1.5",
  bool output_ntuple = true
)
{
  gSystem->Load("libg4dst");
  gSystem->Load("libtrackpid");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  ElectronPid *ePid = new ElectronPid("ElectronPid",outputroot+"_ElectronPid.root");
  ePid->set_output_ntuple(output_ntuple);
 // ePid->Verbosity(1);
  ePid->setEMOPcutlimits(0.7,1.5);
  ePid->setHinOEMcutlimit(0.2);
  ePid->setPtcutlimit(2.0,30.0);
  ePid->setHOPcutlimit(0.3);
  se->registerSubsystem(ePid);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_TRACKS");
  in->Verbosity(1);
 // in->fileopen(inputFile);
  in->AddListFile("filelist.txt");
  se->registerInputManager(in);


  if(!output_ntuple) {
  	Fun4AllOutputManager *outePid = new Fun4AllDstOutputManager("outePid",outputroot+"_ElectronPid_DST.root");
  	outePid->AddNode("TrackPidAssoc");
  	se->registerOutputManager(outePid);
	outePid->Verbosity(1);
 	outePid->Print();
  }

  se->run();
  se->End();

}
