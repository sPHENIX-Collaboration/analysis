// ----------------------------------------------------------------------------
// 'Fun4All_TestTriggerClusterMakerOnSim.C'
// Derek Anderson
// 05.16.2024
//
// A small Fun4All macro to test the 'TriggerClusterMaker' module.
// ----------------------------------------------------------------------------

#define FUN4ALL_TESTTRIGGERCLUSTERMAKER_C

// c++ utilities
#include <string>
#include <vector>
// calotrigger utilities
#include <caloreco/CaloTowerCalib.h>
#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloWaveformProcessing.h>
#include <calotrigger/LL1PacketGetter.h>
#include <calotrigger/CaloTriggerEmulator.h>
// ffa modules
#include <ffamodules/FlagHandler.h>
#include <ffamodules/CDBInterface.h>
// fun4all libraries
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
// phool utilities
#include <phool/recoConsts.h>
// module definitions
#include <triggerclustermaker/TriggerClusterMaker.h>

// load libraries
R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libtriggerclustermaker.so)

// convenience types
typedef std::vector<std::string> SVec;




// macro body -----------------------------------------------------------------

void Fun4All_TestTriggerClusterMakerOnSim(
  const int  runnumber  = 11,
  const int  nEvents    = 10,
  const int  verbosity  = 5,
  const SVec vecInFiles = {
    "input/pp200py8jet10run11.DstCaloCluster.list",
    "input/pp200py8jet10run11.DstG4Hits.list"
  },
  const SVec lutFiles = {
    "/sphenix/user/dlis/Projects/macros/CDBTest/emcal_ll1_lut.root",
    "/sphenix/user/dlis/Projects/macros/CDBTest/hcalin_ll1_lut.root",
    "/sphenix/user/dlis/Projects/macros/CDBTest/hcalout_ll1_lut.root"
  },
  const std::string outFile = "test.root"
) {

  // options ------------------------------------------------------------------

  // tower builder options
  const bool        offline     = true;
  const uint32_t    nSamples    = 12;
  const std::string wavePrefix  = "WAVEFORM_";
  const std::string calibPrefix = "TOWERSWAVEFORM_CALIB_";

  // tower builder type: need waveforms for trigger primitives
  CaloTowerDefs::BuilderType build = CaloTowerDefs::kPRDFWaveform;

  // emulator options
  const bool        useEMCal = true;
  const bool        useIHCal = true;
  const bool        useOHCal = true;
  const uint32_t    iThresh  = 1;
  const uint32_t    nSampUse = 6;
  const uint32_t    nDelay   = 5;
  const std::string type     = "JET";

  // trigger cluster maker options
  TriggerClusterMakerConfig cfg_maker {
    .debug        = true,
    .outNodeName  = "TriggerClusters",
    .inLL1Nodes = {
      "LL1OUT_JET"
    },
    .inPrimNodes = {
      "TRIGGERPRIMITIVES_JET",
      "TRIGGERPRIMITIVES_EMCAL",
      "TRIGGERPRIMITIVES_EMCAL_LL1",
      "TRIGGERPRIMITIVES_HCAL_LL1",
      "TRIGGERPRIMITIVES_HCALIN",
      "TRIGGERPRIMITIVES_HCALOUT"
    },
    .inEMCalTowerNode = "TOWERINFO_CALIB_CEMC",
    .inIHCalTowerNode = "TOWERINFO_CALIB_HCALIN",
    .inOHCalTowerNode = "TOWERINFO_CALIB_HCALOUT"
  };

  // initialize f4a -----------------------------------------------------------

  Fun4AllServer* f4a = Fun4AllServer::instance();
  CDBInterface*  cdb = CDBInterface::instance();
  recoConsts*    rc  = recoConsts::instance();
  f4a -> Verbosity(verbosity);
  cdb -> Verbosity(verbosity);

  // grab lookup tables
  rc -> set_StringFlag("CDB_GLOBALTAG", "ProdA_2023");
  rc -> set_uint64Flag("TIMESTAMP", runnumber);

  // register inputs/outputs and handelers ------------------------------------

  // register input managers
  for (size_t iInput = 0; iInput < vecInFiles.size(); ++iInput) {
    Fun4AllDstInputManager* input = new Fun4AllDstInputManager("InputDstManager" + std::to_string(iInput));
    input -> AddListFile(vecInFiles[iInput]);
    f4a   -> registerInputManager(input);
  }

  // register output manager
  Fun4AllDstOutputManager* output = new Fun4AllDstOutputManager("OutputDstManager", outFile);
  f4a -> registerOutputManager(output);

  // register flag handler
  FlagHandler* handler = new FlagHandler();
  f4a -> registerSubsystem(handler);

  // register subsystem reco modules ------------------------------------------

  // build emcal towers  
  CaloTowerBuilder* emBuilder = new CaloTowerBuilder("EMCALBUILDER");
  emBuilder -> set_detector_type(CaloTowerDefs::CEMC);
  emBuilder -> set_processing_type(CaloWaveformProcessing::FAST);
  emBuilder -> set_builder_type(build);
  emBuilder -> set_outputNodePrefix(wavePrefix);
  emBuilder -> set_nsamples(nSamples);
  emBuilder -> set_offlineflag(offline);
  f4a       -> registerSubsystem(emBuilder);

  // build inner hcal towers
  CaloTowerBuilder* ihBuilder = new CaloTowerBuilder("HCALINBUILDER");
  ihBuilder -> set_detector_type(CaloTowerDefs::HCALIN);
  ihBuilder -> set_processing_type(CaloWaveformProcessing::FAST);
  ihBuilder -> set_builder_type(build);
  ihBuilder -> set_outputNodePrefix(wavePrefix);
  ihBuilder -> set_nsamples(nSamples);
  ihBuilder -> set_offlineflag(offline);
  f4a       -> registerSubsystem(ihBuilder);

  // build outer hcal towers
  CaloTowerBuilder* ohBuilder = new CaloTowerBuilder("HCALOUTBUILDER");
  ohBuilder -> set_detector_type(CaloTowerDefs::HCALOUT);
  ohBuilder -> set_processing_type(CaloWaveformProcessing::FAST);
  ohBuilder -> set_builder_type(build);
  ohBuilder -> set_outputNodePrefix(wavePrefix);
  ohBuilder -> set_nsamples(nSamples);
  ohBuilder -> set_offlineflag(offline);
  f4a       -> registerSubsystem(ohBuilder);

  // calibrate emcal towers
  CaloTowerCalib* emCalib = new CaloTowerCalib();
  emCalib -> set_detector_type(CaloTowerDefs::CEMC);
  emCalib -> set_inputNodePrefix(wavePrefix);
  emCalib -> set_outputNodePrefix(calibPrefix);
  f4a     -> registerSubsystem(emCalib);

  // calibrate inner hcal towers
  CaloTowerCalib* ihCalib = new CaloTowerCalib();
  ihCalib -> set_detector_type(CaloTowerDefs::HCALIN);
  ihCalib -> set_inputNodePrefix(wavePrefix);
  ihCalib -> set_outputNodePrefix(calibPrefix);
  f4a     -> registerSubsystem(ihCalib);

  // calibrate outer hcal towers
  CaloTowerCalib* ohCalib = new CaloTowerCalib();
  ohCalib -> set_detector_type(CaloTowerDefs::HCALOUT);
  ohCalib -> set_inputNodePrefix(wavePrefix);
  ohCalib -> set_outputNodePrefix(calibPrefix);
  f4a     -> registerSubsystem(ohCalib);

  // run emulator
  CaloTriggerEmulator* emulator = new CaloTriggerEmulator("TriggerEmulator");
  emulator -> Verbosity(verbosity);
  emulator -> setTriggerType(type);
  emulator -> setNSamples(nSamples);
  emulator -> setTriggerSample(nSampUse);
  emulator -> useEMCAL(useEMCal);
  emulator -> useHCALIN(useIHCal);
  emulator -> useHCALOUT(useOHCal);
  emulator -> setEmcalLUTFile(lutFiles.at(0));
  emulator -> setHcalinLUTFile(lutFiles.at(1));
  emulator -> setHcaloutLUTFile(lutFiles.at(2));
  emulator -> setThreshold(iThresh);
  f4a      -> registerSubsystem(emulator);

  // finally, make trigger clusters
  TriggerClusterMaker* maker = new TriggerClusterMaker("TriggerClusterMaker");
  maker -> Verbosity(verbosity);
  maker -> SetConfig(cfg_maker);
  f4a   -> registerSubsystem(maker);

  // run modules and exit -----------------------------------------------------

  // run4all
  f4a -> run(nEvents);
  f4a -> End();
  delete f4a;

  // exit
  gSystem -> Exit(0);
  return;

}

// end ------------------------------------------------------------------------
