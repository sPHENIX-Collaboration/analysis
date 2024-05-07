#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <calovalid/TriggerValid.h>
#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <calotrigger/LL1PacketGetter.h>
#include <calotrigger/CaloTriggerEmulator.h>
#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloTowerCalib.h>
#include <caloreco/CaloWaveformProcessing.h>
#include <phool/recoConsts.h>
#include <QA.C>

R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libtriggervalid.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libemulatortreemaker.so)
#endif

void Fun4All_Calo_Emulator(const int runnumber, int nEvents = 0)
{ 

  // example input file
  std::string fname1 = Form("/sphenix/tg/tg01/commissioning/CaloCalibWG/dlis/DST_PRDF-000%d-0000.root", runnumber);;
  Fun4AllServer *se = Fun4AllServer::instance();

  recoConsts *rc = recoConsts::instance();

  //===============
  // conditions DB flags
  //===============

  //CDB to grab Lookup tables
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2023");
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  
  int verbosity = 0;
  se->Verbosity(1);


  // Need waveforms to build the trigger primitives for the emulator
  CaloTowerDefs::BuilderType buildertype = CaloTowerDefs::kPRDFWaveform;

  
  CaloTowerBuilder *ctbEMCal = new CaloTowerBuilder("EMCALBUILDER");
  ctbEMCal->set_detector_type(CaloTowerDefs::CEMC);
  ctbEMCal->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  ctbEMCal->set_builder_type(buildertype);
  ctbEMCal->set_outputNodePrefix("WAVEFORM_");
  ctbEMCal->set_nsamples(12);
  ctbEMCal->set_offlineflag(true);
  se->registerSubsystem(ctbEMCal);

  CaloTowerBuilder *ctbOHCal = new CaloTowerBuilder("HCALOUTBUILDER");
  ctbOHCal->set_detector_type(CaloTowerDefs::HCALOUT);
  ctbOHCal->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  ctbOHCal->set_builder_type(buildertype);
  ctbOHCal->set_outputNodePrefix("WAVEFORM_");
  ctbOHCal->set_nsamples(12);
  ctbOHCal->set_offlineflag(true);
  se->registerSubsystem(ctbOHCal);

  CaloTowerBuilder *ctbIHCal = new CaloTowerBuilder("HCALINBUILDER");
  ctbIHCal->set_detector_type(CaloTowerDefs::HCALIN);
  ctbIHCal->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  ctbIHCal->set_builder_type(buildertype);
  ctbIHCal->set_outputNodePrefix("WAVEFORM_");
  ctbIHCal->set_nsamples(12);
  ctbIHCal->set_offlineflag(true);
  se->registerSubsystem(ctbIHCal);

  CaloTowerCalib *calib = new CaloTowerCalib();
  calib->set_detector_type(CaloTowerDefs::HCALOUT);
  calib->set_inputNodePrefix("WAVEFORM_");  
  calib->set_outputNodePrefix("TOWERSWAVEFORM_CALIB_");
  se->registerSubsystem(calib);

  calib = new CaloTowerCalib();
  calib->set_detector_type(CaloTowerDefs::HCALIN);
  calib->set_inputNodePrefix("WAVEFORM_");  
  calib->set_outputNodePrefix("TOWERSWAVEFORM_CALIB_");
  se->registerSubsystem(calib);

  calib = new CaloTowerCalib();
  calib->set_detector_type(CaloTowerDefs::CEMC);
  calib->set_inputNodePrefix("WAVEFORM_");  
  calib->set_outputNodePrefix("TOWERSWAVEFORM_CALIB_");
  se->registerSubsystem(calib);

  // Calo Trigger Emulator
  CaloTriggerEmulator *te = new CaloTriggerEmulator("CALOTRIGGEREMULATOR_PHOTON");
  te->Verbosity(verbosity);
  // need to set trigger type 
  // "JET" -- Full Jet algorithm
  // "PHOTON" -- only 8x8 patch photon right now
  // "PAIR" -- not implemented
  te->setTriggerType("PHOTON");

  // need number of calo readout samples
  te->setNSamples(12);
  
  // which sample was used (reference to the Calo readout)
  te->setTriggerSample(6);
  // subrtraction delay of the post and pre sample
  te->setTriggerDelay(5);
  te->useEMCAL(true);
  te->useHCALIN(false);
  te->useHCALOUT(false);
  // If specific thresholds arent set (below, this one is used)

  te->setEmcalLUTFile("/sphenix/user/dlis/Projects/macros/CDBTest/emcal_ll1_lut.root");
  //  te->setHcalinLUTFile("/sphenix/user/dlis/Projects/macros/CDBTest/hcalin_ll1_lut.root");
  // te->setHcaloutLUTFile("/sphenix/user/dlis/Projects/macros/CDBTest/hcalout_ll1_lut.root");

  te->setThreshold(1);
  // Threshold for Jet Trigger uses 4 separate ones. (same as the four we have in the real
  // te->setThreshold(int t1, int t2, int t3, int t4);


  se->registerSubsystem(te);

  Fun4AllInputManager *in = new Fun4AllDstInputManager("in");
  in->fileopen(fname1);
  se->registerInputManager(in);


  TriggerValid *tt2 = new TriggerValid("TriggerValid");
  se->registerSubsystem(tt2);

  char outfile_hist[100];
  sprintf(outfile_hist, "/sphenix/tg/tg01/commissioning/CaloCalibWG/dlis/HIST_TRIGGER_QA-%08d-%04d.root", runnumber, 0);
  string fulloutfile_hist = outfile_hist;

  Fun4AllOutputManager *out = new Fun4AllDstOutputManager("TriggerOut", "/sphenix/tg/tg01/commissioning/CaloCalibWG/dlis/DST_EMULATOR.root");
  out->UseFileRule();
  se->registerOutputManager(out);

// Fun4All
//  se->skip();
  se->run(nEvents);
  se->End();
  TString qaname = fulloutfile_hist;
  std::string qaOutputFileName(qaname.Data());
  QAHistManagerDef::saveQARootFile(qaOutputFileName);

  cout << "JOB COMPLETE :)" <<endl;
}
