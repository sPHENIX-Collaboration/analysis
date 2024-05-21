#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <fun4all/SubsysReco.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllUtils.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>
#include <cdbobjects/CDBTTree.h>
#include <hcal_towerid/hcal_towerid.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libtowerid.so)
R__LOAD_LIBRARY(libmbd_io.so)
R__LOAD_LIBRARY(libcalo_reco.so)
R__LOAD_LIBRARY(libcaloana.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libcalotrigger.so)
R__LOAD_LIBRARY(libcentrality.so)

/*
R__LOAD_LIBRARY(libcdbobjects.so)
R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libtowerid.so)
R__LOAD_LIBRARY(libmbd_io.so)
*/
#endif

void Fun4all_HCalBadTowerID(const int nEvents = 5000, const char *inFile = "/sphenix/lustre01/sphnxpro/commissioning/DST_ana399_2023p008/DST_CALO_run1auau_ana399_2023p008-00020859-0001.root", const char *outName = "output/hottowers.root", const std::string icdbname = "test.root", const std::string ocdbname = "test2.root",float adccut_i = 40, float adccut_o = 40, float sigmas_lo = 2, float sigmas_hi = 5, float inner_f = 0.5, float outer_f = 0.5, int pass = 1)
{
  pair<int, int> runseg = Fun4AllUtils::GetRunSegment(inFile);
  int runnumber = runseg.first;
  int segment = runseg.second;
  
  hcal_towerid *calo = new hcal_towerid("hcal_towerid", outName, icdbname,ocdbname,adccut_i,adccut_o,sigmas_lo,sigmas_hi,inner_f,outer_f);

  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();

  if(pass == 0)  
    {
      se->registerSubsystem(calo);
      Fun4AllInputManager *in = new Fun4AllDstInputManager("DSTcalo");
      in->AddFile(inFile);
      se->registerInputManager(in);
      se->run(nEvents);
      calo -> FillHistograms(runnumber,segment);
      se->End();      
      se->PrintTimer();
    }
  else if(pass == 1)
    {  
      calo -> Init(NULL);
      calo -> CalculateCutOffs(runnumber);
      calo -> WriteCDBTree(runnumber);
      //calo -> WriteCDBTree(runnumber);
    }
  std::cout << "All done!" << std::endl;
  gSystem->Exit(0);
}
