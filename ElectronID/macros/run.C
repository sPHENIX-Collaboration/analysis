#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <GlobalVariables.C>

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>

#include </sphenix/u/weihuma/install/include/electronid/ElectronID.h>
#include </sphenix/u/weihuma/install/include/trackpidassoc/TrackPidAssoc.h>


R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libtrackpid.so)
R__LOAD_LIBRARY(libelectronid.so)

#endif

void run(
  const char *inputFile = "/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04594.root",
  const string &outputroot = "/sphenix/user/weihuma/RunOutput/embedDST_sHijing_upsilon_0_20fm",
  bool output_ntuple = true
)
{
  gSystem->Load("libg4dst");
  gSystem->Load("libtrackpid");
  gSystem->Load("libelectronid");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  ElectronID *eid = new ElectronID("Electronid",outputroot+"_ElectronID.root");
  eid->set_output_ntuple(output_ntuple);
  eid->Verbosity(0);
  eid->setTrackcutlimits(1,1,20,10);//nmvtx, nintt, ntpc, quality
  eid->setEMOPcutlimits(0.7,1.5);
  eid->setHinOEMcutlimit(0.2);
  eid->setPtcutlimit(2.0,30.0);
  eid->setHOPcutlimit(0.3);
  eid->setBDTcut(1,1,0.18,0.24);// (ISUSEBDT_p, ISUSEBDT_n, BDT_cut_p, BDT_cut_n)
  se->registerSubsystem(eid);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_TRACKS");
  in->Verbosity(1);
  //in->fileopen(inputFile);
 //in->AddListFile("filelist_0_4d88fm.txt"); //sHijing with Upsilon embeded;
 // in->AddListFile("filelist_0_12fm.txt"); //sHijing with Upsilon embeded;
  in->AddListFile("filelist_0_20fm.txt"); //sHijing with Upsilon embeded;
  
  se->registerInputManager(in);

  if(output_ntuple) {
  	Fun4AllOutputManager *outePid = new Fun4AllDstOutputManager("outePid",outputroot+"_ElectronPid_DST.root");
  	outePid->AddNode("TrackPidAssoc");
  	se->registerOutputManager(outePid);
	outePid->Verbosity(1);
 	outePid->Print();
  }

  se->run();
  se->End();

}
