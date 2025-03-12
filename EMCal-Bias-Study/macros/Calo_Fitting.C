#ifndef CALO_FITTING_H
#define CALO_FITTING_H

#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloWaveformProcessing.h>

#include <fun4all/Fun4AllServer.h>

R__LOAD_LIBRARY(libcalo_reco.so)

void Process_Calo_Fitting()
{
  Fun4AllServer *se = Fun4AllServer::instance();

  CaloTowerDefs::BuilderType buildertype = CaloTowerDefs::kPRDFTowerv4;

  /////////////////
  // build towers
  CaloTowerBuilder *ctbEMCal = new CaloTowerBuilder("EMCalBUILDER");
  ctbEMCal->set_detector_type(CaloTowerDefs::CEMC);
  // ctbEMCal->set_processing_type(CaloWaveformProcessing::TEMPLATE); /*official*/
  ctbEMCal->set_processing_type(CaloWaveformProcessing::FAST);
  ctbEMCal->set_builder_type(buildertype);
  // ctbEMCal->set_offlineflag(true);
  ctbEMCal->set_nsamples(12);
  ctbEMCal->set_bitFlipRecovery(true);
  //60 ADC SZS
  ctbEMCal->set_softwarezerosuppression(true, 60);
  se->registerSubsystem(ctbEMCal);
}

#endif
