#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <GlobalVariables.C>
#include <G4_Input.C>

#include <caloreco/CaloGeomMapping.h>
#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloTowerCalib.h>
#include <caloreco/CaloWaveformProcessing.h>


#include <calowaveformsim/CaloWaveformSim.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>

#include <fun4allraw/Fun4AllPrdfInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>

#include <phool/recoConsts.h>

#include <calotrigger/CaloTriggerEmulator.h>
#include <calovalid/TriggerValid.h>
#include <CaloEmulatorTreeMaker.h>
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libCaloWaveformSim.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libtriggervalid.so)
R__LOAD_LIBRARY(libemulatortreemaker.so)
#endif

void Fun4All_Sim_Emulator(
    const int nEvents = 100,
    const string &inputFile0 = "g4hits_12.list",
    const string &inputFile1 = "dst_calo_cluster_12.list",
    const string &inputFile2 = "/sphenix/user/shuhangli/noisetree/macro/condor31/OutDir0/pedestalhg.root",
    //const string &inputFile2 = "pedestal.root",

    const string &outputFile = "DST_CALO_WAVEFORM_pp-0000000011-00000.root",
    const string &outdir = ".",
    const string &cdbtag = "ProdA_2023")
{

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);

  recoConsts *rc = recoConsts::instance();

  Enable::CDB = true;
  rc->set_StringFlag("CDB_GLOBALTAG", cdbtag);
  rc->set_uint64Flag("TIMESTAMP", 7);
  CDBInterface::instance()->Verbosity(1);

  //===============
  // Input options
  //===============
  // verbosity setting (applies to all input managers)
  Input::VERBOSITY = 1;
  // First enable the input generators
  // Either:
  // read previously generated g4-hits files, in this case it opens a DST and skips
  // the simulations step completely. The G4Setup macro is only loaded to get information
  // about the number of layers used for the cell reco code
  Input::READHITS = true;

  //INPUTREADHITS::filename[0] = inputFile0;
  //INPUTREADHITS::filename[1] = inputFile1;

  INPUTREADHITS::listfile[0] = inputFile0;
  INPUTREADHITS::listfile[1] = inputFile1;

  InputInit();

  // register all input generators with Fun4All
  InputRegister();

// register the flag handling
  FlagHandler *flag = new FlagHandler();
  se->registerSubsystem(flag);

  Enable::DSTOUT = false;
  Enable::DSTOUT_COMPRESS = false;
  DstOut::OutputDir = outdir;
  DstOut::OutputFile = outputFile;

  CaloWaveformSim *caloWaveformSim;

  caloWaveformSim= new CaloWaveformSim();
  caloWaveformSim->set_detector_type(CaloTowerDefs::HCALIN);
  caloWaveformSim->set_detector("HCALIN");
  caloWaveformSim->set_nsamples(16);
  caloWaveformSim->set_highgain(false);
  caloWaveformSim->set_timewidth(0.2);
  caloWaveformSim->set_peakpos(6);
  //caloWaveformSim->set_noise_type(CaloWaveformSim::NOISE_NONE);
  se->registerSubsystem(caloWaveformSim);

  caloWaveformSim= new CaloWaveformSim();
  caloWaveformSim->set_detector_type(CaloTowerDefs::HCALOUT);
  caloWaveformSim->set_detector("HCALOUT");
  caloWaveformSim->set_nsamples(16);
  caloWaveformSim->set_highgain(false);
  caloWaveformSim->set_timewidth(0.2);
  caloWaveformSim->set_peakpos(6);
  //caloWaveformSim->set_noise_type(CaloWaveformSim::NOISE_NONE);
  se->registerSubsystem(caloWaveformSim);

  caloWaveformSim= new CaloWaveformSim();
  caloWaveformSim->set_detector_type(CaloTowerDefs::CEMC);
  caloWaveformSim->set_detector("CEMC");
  caloWaveformSim->set_nsamples(16);
  caloWaveformSim->set_highgain(true);
  caloWaveformSim->set_timewidth(0.2);
  caloWaveformSim->set_peakpos(6);
  caloWaveformSim->set_calibName("cemc_pi0_twrSlope_v1_default");
  se->registerSubsystem(caloWaveformSim);
  
  CaloTowerBuilder *ca2 = new CaloTowerBuilder();
  ca2->set_detector_type(CaloTowerDefs::HCALOUT);
  ca2->set_nsamples(16);
  ca2->set_dataflag(false);
  ca2->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  ca2->set_builder_type(CaloTowerDefs::kWaveformTowerv2);
  se->registerSubsystem(ca2);

  ca2 = new CaloTowerBuilder();
  ca2->set_detector_type(CaloTowerDefs::HCALIN);
  ca2->set_nsamples(16);
  ca2->set_dataflag(false);
  ca2->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  ca2->set_builder_type(CaloTowerDefs::kWaveformTowerv2);
  se->registerSubsystem(ca2);

  ca2 = new CaloTowerBuilder();
  ca2->set_detector_type(CaloTowerDefs::CEMC);
  ca2->set_nsamples(16);
  ca2->set_dataflag(false);
  ca2->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  ca2->set_builder_type(CaloTowerDefs::kWaveformTowerv2);
  //  ca2->set_softwarezerosuppression(true, 300);
  se->registerSubsystem(ca2);


  //tower calib

  CaloTowerCalib *calib = new CaloTowerCalib();
  calib->set_detector_type(CaloTowerDefs::HCALOUT);
  calib->set_outputNodePrefix("TOWERSWAVEFORM_CALIB_");
  se->registerSubsystem(calib);

  calib = new CaloTowerCalib();
  calib->set_detector_type(CaloTowerDefs::HCALIN);
  calib->set_outputNodePrefix("TOWERSWAVEFORM_CALIB_");
  se->registerSubsystem(calib);

  calib = new CaloTowerCalib();
  calib->set_detector_type(CaloTowerDefs::CEMC);
  calib->set_outputNodePrefix("TOWERSWAVEFORM_CALIB_");
  se->registerSubsystem(calib);

  CaloTriggerEmulator *te = new CaloTriggerEmulator("CALOTRIGGEREMULATOR");
  te->setTriggerType("JET");
  te->setNSamples(16);
  te->setTriggerSample(6);
  // subrtraction delay of the post and pre sample
  te->setTriggerDelay(4);
  te->Verbosity(0);
  te->setThreshold(1, 4, 6, 8);
  te->setEmcalLUTFile("/sphenix/user/dlis/Projects/macros/CDBTest/emcal_ll1_lut.root");
  te->setHcalinLUTFile("/sphenix/user/dlis/Projects/macros/CDBTest/hcalin_ll1_lut.root");
  te->setHcaloutLUTFile("/sphenix/user/dlis/Projects/macros/CDBTest/hcalout_ll1_lut.root");
  se->registerSubsystem(te);


  InputManagers();

  Fun4AllInputManager *hitsin = new Fun4AllNoSyncDstInputManager("DST2");
  hitsin->AddFile(inputFile2);
  hitsin->Repeat();
  se->registerInputManager(hitsin);

  CaloEmulatorTreeMaker *tt1 = new CaloEmulatorTreeMaker("CaloEmulatorTreemaker","test1.root");
  tt1->UseCaloTowerBuilder(true);
  tt1->Verbosity(0);
  se->registerSubsystem(tt1);

  TriggerValid *tt2 = new TriggerValid("TriggerValidation","test2.root");
  se->registerSubsystem(tt2);

  
  se->run(nEvents);
  CDBInterface::instance()->Print();  // print used DB files
  se->End();
  se->PrintTimer();
  gSystem->Exit(0);
}
