#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <calovalid/TriggerValid.h>
#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <CaloEmulatorTreeMaker.h>
#include <calotrigger/LL1PacketGetter.h>
#include <calotrigger/CaloTriggerEmulator.h>
#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloWaveformProcessing.h>
#include <phool/recoConsts.h>
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libtriggervalid.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libemulatortreemaker.so)
#endif

void Fun4All_RawData_Emulator(const int runnumber)
{ 

  // example input file
  std::string fname1 = Form("/sphenix/user/dlis/Projects/raw/triggertests/triggertest-000%d-0000.prdf", runnumber);;
  Fun4AllServer *se = Fun4AllServer::instance();

  recoConsts *rc = recoConsts::instance();

  //===============
  // conditions DB flags
  //===============

  //CDB to grab Lookup tables
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2023");
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(1);
  
  int verbosity = 0;
  se->Verbosity(verbosity);


  // Get LL1 Data into NodeTree for Jet LL1 board

  LL1PacketGetter *la = new LL1PacketGetter("LL1PACKETGETTER_JET","JET", "HCAL");
  la->Verbosity(verbosity);
  se->registerSubsystem(la);

  // Get LL1 Data into Nodetree for EMCAL LL1 boards

  LL1PacketGetter *la1 = new LL1PacketGetter("LL1PACKETGETTER_EMCAL","NONE", "EMCAL");
  la1->Verbosity(verbosity);
  se->registerSubsystem(la1);

  // Need waveforms to build the trigger primitives for the emulator
  CaloTowerDefs::BuilderType buildertype = CaloTowerDefs::kPRDFWaveform;

  
  CaloTowerBuilder *ctbEMCal = new CaloTowerBuilder("EMCALBUILDER");
  ctbEMCal->set_detector_type(CaloTowerDefs::CEMC);
  ctbEMCal->set_processing_type(CaloWaveformProcessing::FAST);
  ctbEMCal->set_builder_type(buildertype);
  ctbEMCal->set_outputNodePrefix("WAVEFORM_");
  ctbEMCal->set_nsamples(12);
  se->registerSubsystem(ctbEMCal);

  CaloTowerBuilder *ctbOHCal = new CaloTowerBuilder("HCALOUTBUILDER");
  ctbOHCal->set_detector_type(CaloTowerDefs::HCALOUT);
  ctbOHCal->set_processing_type(CaloWaveformProcessing::FAST);
  ctbOHCal->set_builder_type(buildertype);
  ctbOHCal->set_outputNodePrefix("WAVEFORM_");
  ctbOHCal->set_nsamples(12);
  se->registerSubsystem(ctbOHCal);

  CaloTowerBuilder *ctbIHCal = new CaloTowerBuilder("HCALINBUILDER");
  ctbIHCal->set_detector_type(CaloTowerDefs::HCALIN);
  ctbIHCal->set_processing_type(CaloWaveformProcessing::FAST);
  ctbIHCal->set_builder_type(buildertype);
  ctbIHCal->set_outputNodePrefix("WAVEFORM_");
  ctbIHCal->set_nsamples(12);
  se->registerSubsystem(ctbIHCal);

  // Calo Trigger Emulator
  CaloTriggerEmulator *te = new CaloTriggerEmulator("CALOTRIGGEREMULATOR_JET");
  te->Verbosity(verbosity);
  // need to set trigger type 
  // "JET" -- Full Jet algorithm
  // "PHOTON" -- only 8x8 patch photon right now
  // "PAIR" -- not implemented
  te->setTriggerType("JET");

  // need number of calo readout samples
  te->setNSamples(12);
  
  // which sample was used (reference to the Calo readout)
  te->setTriggerSample(9);
  // subrtraction delay of the post and pre sample
  te->setTriggerDelay(6);

  // If specific thresholds arent set (below, this one is used)
  te->setThreshold(1);
  // Threshold for Jet Trigger uses 4 separate ones. (same as the four we have in the real
  // te->setThreshold(int t1, int t2, int t3, int t4);


  se->registerSubsystem(te);

  Fun4AllInputManager *in = new Fun4AllPrdfInputManager("in");
  in->fileopen(fname1);
  se->registerInputManager(in);

  char outfile[100];
  sprintf(outfile, "/sphenix/user/dlis/Projects/CaloTriggerEmulator/outputs/cosmicjet-%d.root", runnumber);

  char outfile2[100];
  sprintf(outfile2, "/sphenix/user/dlis/Projects/CaloTriggerEmulator/outputs/histos-%d.root", runnumber);

  CaloEmulatorTreeMaker *tt1 = new CaloEmulatorTreeMaker("CaloEmulatorTreemaker",outfile);
  tt1->UseCaloTowerBuilder(true);
  tt1->Verbosity(verbosity);
  se->registerSubsystem(tt1);

  TriggerValid *tt2 = new TriggerValid("TriggerValid",outfile2);
  se->registerSubsystem(tt2);



// Fun4All
//  se->skip();
  se->run(10);
  se->End();
}
