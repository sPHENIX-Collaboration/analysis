#pragma once

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>
#include <ffamodules/CDBInterface.h>
#include <phool/recoConsts.h>

#include <GlobalVariables.C>
#include <G4_Global.C>
#include <G4_Mbd.C>

#include <neutralmesontssa/neutralMesonTSSA.h>

#include <caloreco/CaloTowerCalib.h>
#include <caloreco/CaloTowerStatus.h>
#include <caloreco/RawClusterBuilderTemplate.h>
#include <caloreco/RawClusterPositionCorrection.h>

#include <Calo_Calib.C>

#include <fstream>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libneutralMesonTSSA.so)

std::string GetFirstFile(const char* filelist)
{
  std::string firstfile = "";
  ifstream in(filelist);
  if (in.good())
  {
    std::getline(in, firstfile);
  }
  return firstfile;
}

void Fun4All_checkHotMaps(
                     int nEvents = 1,
                     const char *filelist1 = "dst_calo_cluster.list",
		     const string outname = "dummyhists.root")
{
  // this convenience library knows all our i/o objects so you don't
  // have to figure out what is in each dst type
  gSystem->Load("libg4dst.so");
  /* gSystem->Load("libneutralMesonTSSA.so"); */

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);  // set it to 1 if you want event printouts
  CDBInterface::instance()->Verbosity(1);

  // conditions DB flags and timestamp
  recoConsts *rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
  std::string firstfile = GetFirstFile(filelist1);
  pair<int, int> runseg = Fun4AllUtils::GetRunSegment(firstfile);
  int runnumber = runseg.first;
  rc->set_uint64Flag("TIMESTAMP", runnumber);

  Fun4AllInputManager *inCluster = new Fun4AllDstInputManager("DSTCaloCluster");
  std::cout << "Adding file list " << filelist1 << std::endl;
  inCluster->AddListFile(filelist1,1);
  se->registerInputManager(inCluster);

  // Tower calibrations
  /* Process_Calo_Calib();  // this line handles the calibrations, dead/hot tower masking and reruns the clusterizer */
  CaloTowerStatus *statusEMC = new CaloTowerStatus("TowerStatusEMC");
  /* statusEMC->Verbosity(1); */
  statusEMC->set_detector_type(CaloTowerDefs::CEMC);
  statusEMC->set_doAbortNoHotMap(true);
  /* statusEMC->set_time_cut(1); */
  se->registerSubsystem(statusEMC);

  /* CaloTowerCalib *calibEMC = new CaloTowerCalib("CEMCCALIB"); */
  /* calibEMC->set_detector_type(CaloTowerDefs::CEMC); */
  /* calibEMC->set_directURL("/sphenix/u/bseidlitz/work/temp24Calib/emcalCalib_withMask_may25.root"); */
  /* se->registerSubsystem(calibEMC); */

  // Clusterizer
  /*
  RawClusterBuilderTemplate *ClusterBuilder = new RawClusterBuilderTemplate("EmcRawClusterBuilderTemplate");
  ClusterBuilder->Detector("CEMC");
  ClusterBuilder->set_threshold_energy(0.03);  // for when using basic calibration
  std::string emc_prof = getenv("CALIBRATIONROOT");
  emc_prof += "/EmcProfile/CEMCprof_Thresh30MeV.root";
  ClusterBuilder->LoadProfile(emc_prof);
  ClusterBuilder->set_UseTowerInfo(1);  // to use towerinfo objects rather than old RawTower
  ClusterBuilder->setOutputClusterNodeName("CLUSTERINFO_CEMC2");
  se->registerSubsystem(ClusterBuilder);
  */

  neutralMesonTSSA *eval = new neutralMesonTSSA("neutralMesonTSSA", outname, false);
  /* eval->set_min_clusterE(0.5); */
  /* eval->set_max_clusterChi2(4.0); */
  se -> registerSubsystem(eval);
  
  se->run(nEvents);
  se->End();
  
  delete se;
  cout << "Analysis Completed" << endl;
  
  gSystem->Exit(0);
}

