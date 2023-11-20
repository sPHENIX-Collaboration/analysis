#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <phool/recoConsts.h>
#include <fun4all/Fun4AllNoSyncDstInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <caloreco/RawClusterBuilderTemplate.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>

#include <caloreco/CaloTowerBuilder.h>
#include <caloreco/CaloWaveformProcessing.h>

#include <ffamodules/FlagHandler.h>
#include <ffamodules/HeadReco.h>
#include <ffamodules/SyncReco.h>
#include <ffamodules/CDBInterface.h>

#include <caloreco/TowerInfoDeadHotMask.h>
#include <caloreco/DeadHotMapLoader.h>
#include <caloreco/RawClusterDeadHotMask.h>


#include <caloana/CaloAna.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllUtils.h>
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libcaloana.so)
R__LOAD_LIBRARY(libffamodules.so)
#endif

void Fun4All_Calo(int nevents = 100,const std::string &fname = "input.txt")
{

  bool enableMasking = 0;

  Fun4AllServer *se = Fun4AllServer::instance();
  int verbosity = 0;

  se->Verbosity(verbosity);
  recoConsts *rc = recoConsts::instance();

  ifstream file(fname);
  string first_file;
  getline(file, first_file);

 //===============
  // conditions DB flags
  //===============
  pair<int, int> runseg = Fun4AllUtils::GetRunSegment(first_file);
  int runnumber = runseg.first;
   cout << "run number = " << runnumber << endl;

  // global tag
  rc->set_StringFlag("CDB_GLOBALTAG","MDC2");
  // // 64 bit timestamp
  rc->set_uint64Flag("TIMESTAMP",runnumber);


  Fun4AllInputManager *in = new Fun4AllDstInputManager("DST_TOWERS");
  in->AddListFile(fname);
  se->registerInputManager(in);

  std::string filename = first_file.substr(first_file.find_last_of("/\\") + 1);
  std::string OutFile = "CALOHIST_" + filename;


  /////////////
  // masking
  if (enableMasking)
  {
    std::cout << "Loading EMCal deadmap" << std::endl;
    DeadHotMapLoader *towerMapCemc = new DeadHotMapLoader("CEMC");
    towerMapCemc->detector("CEMC");
    se->registerSubsystem(towerMapCemc);

    std::cout << "Loading ihcal deadmap" << std::endl;
    DeadHotMapLoader *towerMapHCalin = new DeadHotMapLoader("HCALIN");
    towerMapHCalin->detector("HCALIN");
    se->registerSubsystem(towerMapHCalin);

    std::cout << "Loading ohcal deadmap" << std::endl;
    DeadHotMapLoader *towerMapHCalout = new DeadHotMapLoader("HCALOUT");
    towerMapHCalout->detector("HCALOUT");
    se->registerSubsystem(towerMapHCalout);

    std::cout << "Loading cemc masker" << std::endl;
    TowerInfoDeadHotMask *towerMaskCemc = new TowerInfoDeadHotMask("CEMC");
    towerMaskCemc->detector("CEMC");
    se->registerSubsystem(towerMaskCemc);

    std::cout << "Loading hcal maskers" << std::endl;
    TowerInfoDeadHotMask *towerMaskHCalin = new TowerInfoDeadHotMask("HCALIN");
    towerMaskHCalin->detector("HCALIN");
    se->registerSubsystem(towerMaskHCalin);

    TowerInfoDeadHotMask *towerMaskHCalout = new TowerInfoDeadHotMask("HCALOUT");
    towerMaskHCalout->detector("HCALOUT");
    se->registerSubsystem(towerMaskHCalout);

    ///////////
    // Clusters
    std::cout << "Building clusters" << std::endl;
    RawClusterBuilderTemplate *ClusterBuilder = new RawClusterBuilderTemplate("EmcRawClusterBuilderTemplate");
    ClusterBuilder->Detector("CEMC");
    ClusterBuilder->set_threshold_energy(0.030);  // for when using basic calibration
    std::string emc_prof = getenv("CALIBRATIONROOT");
    emc_prof += "/EmcProfile/CEMCprof_Thresh30MeV.root";
    ClusterBuilder->LoadProfile(emc_prof);
    ClusterBuilder->set_UseTowerInfo(1);  // to use towerinfo objects rather than old RawTower
    se->registerSubsystem(ClusterBuilder);


    std::cout << "Masking clusters" << std::endl;
    RawClusterDeadHotMask *clusterMask = new RawClusterDeadHotMask("clusterMask");
    clusterMask->detector("CEMC");
    se->registerSubsystem(clusterMask);

  }

  CaloAna *ca = new CaloAna("calomodulename",OutFile);
  ca->set_timing_cut_width(200);  //integers for timing width, > 1 : wider cut around max peak time
  ca->apply_vertex_cut(false);  
  ca->set_vertex_cut(20.);
  se->registerSubsystem(ca);

  se->run(nevents); //update number of events as needed
  se->End();

}
