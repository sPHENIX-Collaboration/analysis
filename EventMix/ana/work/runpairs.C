#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include </sphenix/u/weihuma/install/include/trackpidassoc/ElectronPid.h>
#include </sphenix/u/weihuma/install/include/trackpidassoc/TrackPidAssoc.h>
#include </sphenix/u/weihuma/install/include/sphanalysis/sPHAnalysis.h>
#include </sphenix/u/weihuma/install/include/eventmix/PairMaker.h>
#include </sphenix/u/weihuma/install/include/eventmix/sPHElectronPair.h>
#include </sphenix/u/weihuma/install/include/eventmix/sPHElectronPairv1.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libtrackpid.so)
R__LOAD_LIBRARY(libeventmix.so)
R__LOAD_LIBRARY(libsphanalysis.so)
#endif

//void run(const char *fname = "/direct/phenix+u/workarea/lebedev/sPHENIX_new/analysis/EventMix/macro/hijing.root")
//void runpairs(const char *fname = "/sphenix/user/lebedev/mdc/test.root")
//void runpairs(const char *fname = "/sphenix/u/weihuma/RunOutput/EVENTMIX/ana/Ana_Upsilon_electrons_cutting_0_20fm.root")
//void runpairs(const char *fname = "/sphenix/u/weihuma/RunOutput/EVENTMIX/ana/Ana_Upsilon_0_20fm.root")
//void runpairs(const char *fname = "/sphenix/u/weihuma/RunOutput/EVENTMIX/ana/Ana_Upsilon_0_20fm_change_Eop_pt_cut.root")
//void runpairs(const char *fname = "/sphenix/u/weihuma/RunOutput/EVENTMIX/ana/Ana_Upsilon_electrons_cutting_0_20fm_change_Eop_pt_cut.root")
void runpairs(const char *fname = "/sphenix/u/weihuma/RunOutput/EVENTMIX/Ana_Upsilon_embed_sHijing_0_20fm_type1_TrackPidAssoc_toBJ.root",
bool output_ntuple = true)
{
  gSystem->Load("libg4dst");
  gSystem->Load("libeventmix");
  gSystem->Load("libsphanalysis");
  gSystem->Load("libtrackpid");

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  ElectronPid* eid = new ElectronPid("ElectronPid","/sphenix/u/weihuma/RunOutput/EVENTMIX/Ana_Upsilon_embed_sHijing_0_20fm_type1_with_eid_TrackPidAssoc_ntuple_toBJ.root");
  eid->Verbosity(1);
  eid->set_output_ntuple(output_ntuple);
 // eid->setEMOPcutlimits(0.7,1.5);
  eid->setEMOPcutlimits(0.2,1.5);
  //eid->setHinOEMcutlimit(0.2);
  eid->setPtcutlimit(2.0,30.0);
  eid->setHOPcutlimit(0.3);
  se->registerSubsystem(eid);

  PairMaker *pmaker = new PairMaker("PairMaker","dummy.root");
  pmaker->Verbosity(1);
  se->registerSubsystem(pmaker);

  sPHAnalysis *ana = new sPHAnalysis("sPHAnalysis",fname);
  ana->Verbosity(1);
  se->registerSubsystem(ana);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
  in->Verbosity(1);
  se->registerInputManager(in);
//  in->AddFile("/sphenix/u/weihuma/RunOutput/embedDST_sHijing_upsilon_0_20fm_ElectronPid_DST_0000.root");
 // in->AddFile("/sphenix/u/weihuma/RunOutput/embedDST_sHijing_upsilon_0_20fm_ElectronPid_DST.root");
  
//in->AddListFile("pythiaupsilons.txt");
  //in->AddFile("/sphenix/user/lebedev/mdc/eePairs_test.root");
//  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/eePairs_mb2010.root");
//  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/eePairs_mb2020.root");
  //in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/eePairs_mb2030.root");
  //in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/eePairs_mb2040.root");
  //in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/eePairs_mb2050.root");
  //in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/eePairs_mb2060.root");
  //in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/eePairs_mb2070.root");
  //in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/eePairs_mb2080.root");
  //in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/eePairs_mb2090.root");
  //in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/eePairs_mb2100.root");

  //in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-02990.root");
  //in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-02991.root");

  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04594.root"); 
  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04595.root");
  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04596.root");
  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04597.root");
  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04598.root");
  in->AddFile("/sphenix/sim/sim01/sphnxpro/MDC1/embed/embedDST_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000001-04599.root");

/*
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04960.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04961.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04962.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04963.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04964.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04965.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04966.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04967.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04968.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04969.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04970.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04971.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04972.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04973.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04974.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04975.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04976.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04977.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04978.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04979.root");
*/
/*
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04980.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04981.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04982.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04983.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04984.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04985.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04986.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04987.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04988.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04989.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04980.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04991.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04992.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04993.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04994.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04995.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04996.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04997.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04998.root");
  in->AddFile("/sphenix/user/lebedev/mdc/embed/DST_sHijing_0_488fm-0000000001-04999.root");
*/

/*
  in->AddFile("/sphenix/user/lebedev/mdc/sPHENIX_pythiaupsilons_110.root");
  in->AddFile("/sphenix/user/lebedev/mdc/sPHENIX_pythiaupsilons_111.root");
  in->AddFile("/sphenix/user/lebedev/mdc/sPHENIX_pythiaupsilons_112.root");
  in->AddFile("/sphenix/user/lebedev/mdc/sPHENIX_pythiaupsilons_113.root");
  in->AddFile("/sphenix/user/lebedev/mdc/sPHENIX_pythiaupsilons_114.root");
  in->AddFile("/sphenix/user/lebedev/mdc/sPHENIX_pythiaupsilons_115.root");
  in->AddFile("/sphenix/user/lebedev/mdc/sPHENIX_pythiaupsilons_116.root");
  in->AddFile("/sphenix/user/lebedev/mdc/sPHENIX_pythiaupsilons_117.root");
  in->AddFile("/sphenix/user/lebedev/mdc/sPHENIX_pythiaupsilons_117.root");
  in->AddFile("/sphenix/user/lebedev/mdc/sPHENIX_pythiaupsilons_119.root");
  //in->AddFile("/sphenix/user/lebedev/mdc/pythiaupsilons/sPHENIX_pythiaupsilons_0.root");
  //in->AddFile("/sphenix/user/lebedev/mdc/pythiaupsilons/sPHENIX_pythiaupsilons_1.root");
*/
/*
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_0.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_1.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_2.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_3.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_4.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_5.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_6.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_7.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_8.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_9.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_10.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_11.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_12.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_13.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_14.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_15.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_16.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_17.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_18.root");
  in->AddFile("/sphenix/user/lebedev/mdc/G4sPHENIX_19.root");
*/

  se->run();
  se->End();
}

