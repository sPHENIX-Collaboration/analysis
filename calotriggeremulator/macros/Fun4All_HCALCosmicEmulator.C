#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>

#include <caloreco/CaloTowerBuilder.h>

#include <calotrigger/CaloTriggerEmulator.h>
#include <HCALEmulatorTreeMaker.h>
#include <phool/recoConsts.h>

R__LOAD_LIBRARY(libhcalemulatortreemaker.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libhcalemulatortreemaker.so)

#endif

void Fun4All_HCALCosmicEmulator(const std::string &fname1 = "/sphenix/user/dlis/Projects/raw/cosmic/cosmics_HCAL_00032029-0000.prdf", const char *outfile1 = "trees_32029.root", const char *outfile2 = "trees2_32029.root")
{
  gSystem->Load("libg4dst");
  gSystem->Load("libcaloana");
  gSystem->Load("libcalotrigger");
  gSystem->Load("libhcalemulatortreemaker");

  int runnumber = 32029;
  int verbosity = 0;
  int nevents = 10;
  if (FILE *file = fopen(fname1.c_str(),"r")){
    fclose(file);
  }
  else
    {
      std::cout << "NOOOOO ... no "<< fname1 <<std::endl;
      return;
    }

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  //===============
  // conditions DB flags
  //===============
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2023");
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance()->Verbosity(1);


  CaloTowerDefs::BuilderType buildertype = CaloTowerDefs::kPRDFWaveform;

  CaloTowerBuilder *ctbIHCal = new CaloTowerBuilder("HCALINBUILDER");
  ctbIHCal->set_detector_type(CaloTowerDefs::HCALIN);
  ctbIHCal->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  ctbIHCal->set_builder_type(buildertype);
  ctbIHCal->set_outputNodePrefix("WAVEFORM_");
  ctbIHCal->set_nsamples(31);
  se->registerSubsystem(ctbIHCal);

  CaloTowerBuilder *ctbOHCal = new CaloTowerBuilder("HCALOUTBUILDER");
  ctbOHCal->set_detector_type(CaloTowerDefs::HCALOUT);
  ctbOHCal->set_processing_type(CaloWaveformProcessing::TEMPLATE);
  ctbOHCal->set_builder_type(buildertype);
  ctbOHCal->set_outputNodePrefix("WAVEFORM_");
  ctbOHCal->set_nsamples(31);
  se->registerSubsystem(ctbOHCal);

  CaloTriggerEmulator *te = new CaloTriggerEmulator("CALOTRIGGEREMULATOR_COSMIC",outfile1);
  te->Verbosity(verbosity);
  te->setTriggerType("COSMIC");
  te->useHCALIN(false);
  te->setNSamples(12);
  te->setThreshold(3);
  se->registerSubsystem(te);


  HCALEmulatorTreeMaker *tt1 = new HCALEmulatorTreeMaker("HCALEMULATORTREEMAKER",outfile2, "LL1OUT_COSMIC");
  tt1->UseCaloTowerBuilder(true);
  tt1->Verbosity(verbosity);
  se->registerSubsystem(tt1);

  Fun4AllInputManager *in = new Fun4AllPrdfInputManager("in");
  in->fileopen(fname1);
  se->registerInputManager(in);

// Fun4All

  se->run(nevents);
  se->End();
}
