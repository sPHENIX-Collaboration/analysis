#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <frog/FROG.h>

#include <GlobalVariables.C>
#include <G4_Input.C>
#include <QA.C>
#include <caloreco/CaloGeomMapping.h>
#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloTowerCalib.h>
#include <caloreco/CaloWaveformProcessing.h>


#include <calowaveformsim/CaloWaveformSim.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>

#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>

#include <phool/recoConsts.h>
//////// FOR CALOCALIB //////////
#include <calotrigger/CaloTriggerEmulator.h>
#include <calotrigger/TriggerRunInfoReco.h>
//////////////////

#include <calowaveformsim/TriggerPerformance.h>
#include <CaloEmulatorTreeMaker.h>
R__LOAD_LIBRARY(libFROG.so)
R__LOAD_LIBRARY(libdlutility.so)
  R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libCaloWaveformSim.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libffamodules.so)
//////// FOR CALOCALIB //////////
  R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libtriggervalid.so)
R__LOAD_LIBRARY(libemulatortreemaker.so)
#endif

void Fun4All_Sim(
		 const std::string filename = "DST_CALO_CLUSTER_pythia8_Detroit-0000000021-000000.root",
		 const int nEvents = 100)
{

  // example input file
  gSystem->Load("libFROG");

  FROG *fr = new FROG();

  std::string input_1 = fr->location(filename.c_str());

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  recoConsts *rc = recoConsts::instance();


  std::pair<int, int> run_seg = Fun4AllUtils::GetRunSegment(filename.c_str());
  int runnumber = run_seg.first;
  int segment = run_seg.second;

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP", 21);
  CDBInterface::instance()->Verbosity(1);

  //===============
  // Input options
  //===============
  // verbosity setting (applies to all input managers)

  // register the flag handling
  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  //////// FOR CALOCALIB //////////
  TriggerRunInfoReco *trr = new TriggerRunInfoReco("TriggerRunInfoReco");
  trr->UseEmulator(true);  
  trr->Verbosity(0);
  se->registerSubsystem(trr);
  //////// FOR CALOCALIB //////////
  CaloTriggerEmulator *te = new CaloTriggerEmulator("CALOTRIGGEREMULATOR");
  te->setNSamples(12);
  te->setTriggerSample(6);
  te->setTriggerDelay(5);
  te->SetIsData(false);
  te->setEmcalLUTFile("/sphenix/user/dlis/Projects/macros/CDBTest/emcal_ll1_lut_0.50tr_new.root");
  te->setHcalinLUTFile("/sphenix/user/dlis/Projects/macros/CDBTest/hcalin_ll1_lut_0.50tr_new.root");
  te->setHcaloutLUTFile("/sphenix/user/dlis/Projects/macros/CDBTest/hcalout_ll1_lut_0.50tr_new.root");
  se->registerSubsystem(te);
  /////////////////////////////////


  TriggerPerformance *tp = new TriggerPerformance("TriggerPerformance","histo_trigger.root");
  tp->UseEmulator(true);
  tp->Verbosity(0);
  se->registerSubsystem(tp);
  //  std::string trigger = "HIJET";

  // char outfile_hist[1000];
  //  sprintf(outfile_hist, "/sphenix/tg/tg01/commissioning/CaloCalibWG/dlis/HIST_%s_TRIGGER_QA-%08d-%04d.root", trigger.c_str(), runnumber, segment);
  //  string fulloutfile_hist = outfile_hist;


  //  EmulatorHistos *tt2 = new EmulatorHistos("EmulatorHistos");
  //  se->registerSubsystem(tt2);

  // char outfile_dst[100];
  // sprintf(outfile_dst, "/sphenix/tg/tg01/commissioning/CaloCalibWG/dlis/DST_EMULATOR-%08d-%04d.root", runnumber, segment);
  // string fulloutfile_dst = outfile_dst;

  // char outfile_tree[1000];
  // sprintf(outfile_tree, "/sphenix/tg/tg01/commissioning/CaloCalibWG/dlis/TREE_%s_TRIGGER_EMU-%08d-%04d.root", trigger.c_str(), runnumber, segment);
  // string fulloutfile_tree = outfile_tree;

  // CaloEmulatorTreeMaker *tt1 = new CaloEmulatorTreeMaker("CaloEmulatorTreemaker",fulloutfile_tree);
  // tt1->UseCaloTowerBuilder(true);
  // tt1->UseLL1(true);
  // tt1->SetIsSim(true);
  // tt1->Verbosity(0);
  // se->registerSubsystem(tt1);
  Fun4AllInputManager *in1 = new Fun4AllDstInputManager("in1");
  in1->fileopen(input_1.c_str());
  in1->Verbosity(0);
  se->registerInputManager(in1);

  std::cout << "Adding Geometry file" << std::endl;
  Fun4AllInputManager *intrue2 = new Fun4AllRunNodeInputManager("DST_GEO");
  std::string geoLocation = CDBInterface::instance()->getUrl("calo_geo");
  intrue2->AddFile(geoLocation);
  se->registerInputManager(intrue2);
  
  se->run(nEvents);

  se->End();

  // TString qaname = fulloutfile_hist;
  // std::string qaOutputFileName(qaname.Data());
  // QAHistManagerDef::saveQARootFile(qaOutputFileName);

  se->PrintTimer();
  gSystem->Exit(0);
}
