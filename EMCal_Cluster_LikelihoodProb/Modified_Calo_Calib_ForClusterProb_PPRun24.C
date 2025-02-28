#ifndef CALO_CALIB_H
#define CALO_CALIB_H

#include <caloreco/CaloTowerCalib.h>
#include <caloreco/CaloTowerStatus.h>
#include <caloreco/RawClusterBuilderTemplate.h>
#include <caloreco/RawClusterLikelihoodProfile.h>
#include <caloreco/RawClusterDeadHotMask.h>
#include <caloreco/RawClusterPositionCorrection.h>

#include <ffamodules/CDBInterface.h>
#include <ffamodules/FlagHandler.h>
#include <phool/recoConsts.h>

#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllRunNodeInputManager.h>


R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libfun4allutils.so)

void Modified_Calo_Calib_ForClusterProb_PPRun24()
{
  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();
  
  /////////////////
  // set MC or data 
  bool isSim = true;
  int data_sim_runnumber_thres = 1000;
  if (rc->get_uint64Flag("TIMESTAMP") > data_sim_runnumber_thres) 
  {
    isSim = false;
  }
  std::cout << "Calo Calib uses runnumber " << rc->get_uint64Flag("TIMESTAMP") << std::endl;

  //////////////////////////////
  // set statuses on raw towers
  std::cout << "status setters" << std::endl;
  CaloTowerStatus *statusEMC = new CaloTowerStatus("CEMCSTATUS");
  statusEMC->set_detector_type(CaloTowerDefs::CEMC);
  statusEMC->set_time_cut(1);
  se->registerSubsystem(statusEMC);

  CaloTowerStatus *statusHCalIn = new CaloTowerStatus("HCALINSTATUS");
  statusHCalIn->set_detector_type(CaloTowerDefs::HCALIN);
  statusHCalIn->set_time_cut(2);
  se->registerSubsystem(statusHCalIn);

  CaloTowerStatus *statusHCALOUT = new CaloTowerStatus("HCALOUTSTATUS");
  statusHCALOUT->set_detector_type(CaloTowerDefs::HCALOUT);
  statusHCALOUT->set_time_cut(2);
  se->registerSubsystem(statusHCALOUT);

  ////////////////////
  // Calibrate towers
  std::cout << "Calibrating EMCal" << std::endl;
  CaloTowerCalib *calibEMC = new CaloTowerCalib("CEMCCALIB");
  calibEMC->set_detector_type(CaloTowerDefs::CEMC);
  se->registerSubsystem(calibEMC);

  std::cout << "Calibrating OHcal" << std::endl;
  CaloTowerCalib *calibOHCal = new CaloTowerCalib("HCALOUT");
  calibOHCal->set_detector_type(CaloTowerDefs::HCALOUT);
  se->registerSubsystem(calibOHCal);

  std::cout << "Calibrating IHcal" << std::endl;
  CaloTowerCalib *calibIHCal = new CaloTowerCalib("HCALIN");
  calibIHCal->set_detector_type(CaloTowerDefs::HCALIN);
  se->registerSubsystem(calibIHCal);

  ////////////////
  // MC Calibration
  if (isSim) 
  {
    std::string MC_Calib = CDBInterface::instance()->getUrl("CEMC_MC_RECALIB");
    if (MC_Calib.empty())
    {
      std::cout << "No MC calibration found :( )" << std::endl;
      gSystem->Exit(0);
    }
    CaloTowerCalib *calibEMC_MC = new CaloTowerCalib("CEMCCALIB_MC");
    calibEMC_MC->set_detector_type(CaloTowerDefs::CEMC);
    calibEMC_MC->set_inputNodePrefix("TOWERINFO_CALIB_");
    calibEMC_MC->set_outputNodePrefix("TOWERINFO_CALIB_");
    calibEMC_MC->set_directURL(MC_Calib);
    calibEMC_MC->set_doZScrosscalib(false);
    se->registerSubsystem(calibEMC_MC);
  }

  //////////////////
  // Clusters
  std::cout << "Building clusters" << std::endl;
  
  //Default cluster
  RawClusterBuilderTemplate *ClusterBuilder = new RawClusterBuilderTemplate("EmcRawClusterBuilderTemplate");
  ClusterBuilder->Detector("CEMC");
  ClusterBuilder->set_threshold_energy(0.070); 
  std::string emc_prof = getenv("CALIBRATIONROOT");
  emc_prof += "/EmcProfile/CEMCprof_Thresh30MeV.root";
  std::cout << emc_prof.c_str() << std::endl;
  ClusterBuilder->LoadProfile(emc_prof);
  ClusterBuilder->set_UseTowerInfo(1);
  ClusterBuilder->setSubclusterSplitting(true); // Cluster splitting on
  ClusterBuilder->Verbosity(1);

  //Mother cluster
  RawClusterBuilderTemplate *ClusterBuilderMother = new RawClusterBuilderTemplate("EmcRawClusterBuilderTemplateMother");
  ClusterBuilderMother->Detector("CEMC");
  ClusterBuilderMother->set_threshold_energy(0.070); 
  ClusterBuilderMother->LoadProfile(emc_prof);
  ClusterBuilderMother->set_UseTowerInfo(1); 
  ClusterBuilderMother->setSubclusterSplitting(false); // Cluster splitting off
  ClusterBuilderMother->set_outputNodeName("CEMC_CLUSTERINFO_MOTHER");
  ClusterBuilderMother->Verbosity(1);

  //Default cluster profile
  std::string probprofile = CDBInterface::instance()->getUrl("EMCPROBPROFILE");
  std::cout << "probprofile : " << probprofile.c_str() << std::endl;
  RawClusterLikelihoodProfile *ClusterProfile = new RawClusterLikelihoodProfile("RawClusterLikelihoodProfile");
  ClusterProfile->set_profile_filepath(probprofile);
  ClusterProfile->set_min_cluster_e(1.0);
  ClusterProfile->set_inputNodeName("CEMC_CLUSTERINFO");
  ClusterProfile->set_outputNodeName("CEMC_CLUSTERINFO");

  //Mother cluster profile
  RawClusterLikelihoodProfile *ClusterProfileMother= new RawClusterLikelihoodProfile("RawClusterLikelihoodProfileMother");
  ClusterProfileMother->set_profile_filepath(probprofile);
  ClusterProfileMother->set_min_cluster_e(1.0);
  ClusterProfileMother->set_inputNodeName("CEMC_CLUSTERINFO_MOTHER");
  ClusterProfileMother->set_outputNodeName("CEMC_CLUSTERINFO_MOTHER");

  se->registerSubsystem(ClusterBuilder);
  se->registerSubsystem(ClusterBuilderMother);
  se->registerSubsystem(ClusterProfile);
  se->registerSubsystem(ClusterProfileMother);

  // currently NOT included! 
  //std::cout << "Applying Position Dependent Correction" << std::endl;
  //RawClusterPositionCorrection *clusterCorrection = new RawClusterPositionCorrection("CEMC");
  //clusterCorrection->set_UseTowerInfo(1);  // to use towerinfo objects rather than old RawTower
 // se->registerSubsystem(clusterCorrection);
}

#endif
