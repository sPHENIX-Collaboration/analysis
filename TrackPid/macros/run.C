#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <GlobalVariables.C>

#include <DisplayOn.C>
#include <G4Setup_sPHENIX.C>
#include <G4_Bbc.C>
#include <G4_CaloTrigger.C>
#include <G4_DSTReader.C>
#include <G4_Global.C>
#include <G4_HIJetReco.C>
#include <G4_Input.C>
#include <G4_Jets.C>
#include <G4_ParticleFlow.C>
#include <G4_Production.C>
#include <G4_TopoClusterReco.C>
#include <G4_Tracking.C>
#include <G4_User.C>

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
  const char *fname = "/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_12fm_50kHz_bkg_0_12fm-0000000001-01998.root",
  const string &outputroot = "embedDST_sHijing_upsilon_0_12fm_EOP_0.7_1.5"
)
{
  gSystem->Load("libg4dst");
  gSystem->Load("libtrackpid");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  ElectronPid *ePid = new ElectronPid("ElectronPid");
  ePid->Verbosity(1);
  ePid->setEOPcutlimits(0.7,1.5);
  ePid->setHOPcutlimit(0.5);
  se->registerSubsystem(ePid);

  AnaTutorial *anaTutorial = new AnaTutorial("anaTutorial", outputroot + "_anaTutorial.root");
  anaTutorial->setMinJetPt(10.);
  anaTutorial->Verbosity(1);
  anaTutorial->analyzeTracks(true);
  anaTutorial->analyzeClusters(false);
  anaTutorial->analyzeJets(false);
  anaTutorial->analyzeTruth(false);
  //se->registerSubsystem(anaTutorial);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_TRACKS");
  in->Verbosity(1);
  in->fileopen(fname);
 // in->AddListFile("filelist.txt");
  se->registerInputManager(in);


 // if(!write_ntuple) {
  	Fun4AllOutputManager *outePid = new Fun4AllDstOutputManager("outePid","embedDST_sHijing_upsilon_0_12fm_EOP_0.7_1.5.root");
  	outePid->AddNode("TrackPidAssoc");
  	se->registerOutputManager(outePid);
	outePid->Verbosity(1);
 	outePid->Print();
  //}

  se->run();
  outePid->Print();

  se->End();

}
