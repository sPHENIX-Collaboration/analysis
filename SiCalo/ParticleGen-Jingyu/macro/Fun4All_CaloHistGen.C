#ifndef MACRO_FUN4ALL_CALOTREEGEN_C
#define MACRO_FUN4ALL_CALOTREEGEN_C

#include <ffamodules/CDBInterface.h>
#include <fun4all/Fun4AllUtils.h>

#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHRandomSeed.h>
#include <phool/recoConsts.h>

#include <tutorial/caloHistGen.h>
#include <tutorial/CaloOnly.h>

#include <Calo_Calib.C>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libffarawobjects.so)
// R__LOAD_LIBRARY(libcaloHistGen.so)
R__LOAD_LIBRARY( libtutorial.so )

  void Fun4All_CaloHistGen(const int nEvents = 100, const std::string &fnameCalib = "DST_CALO_run2pp_ana450_2024p009-00053018-00001.root", const std::string &fnameRaw = "DST_CALOFITTING_run2pp_ana446_2024p007-00053018-00001.root",const std::string &outName = "commissioning.root", const std::string &dbtag = "ProdA_2024", const std::string &outNamejy = "commissioning.root")
{
  Fun4AllServer *se = Fun4AllServer::instance();
  recoConsts *rc = recoConsts::instance();
  
  pair<int, int> runseg = Fun4AllUtils::GetRunSegment(fnameCalib);
  int runnumber = runseg.first;

  rc -> set_StringFlag("CDB_GLOBALTAG",dbtag);
  rc -> set_uint64Flag("TIMESTAMP",runnumber);
  CDBInterface::instance() -> Verbosity(1);

  gSystem->Load("libg4dst");
  
  Fun4AllInputManager *inRaw = new Fun4AllDstInputManager("DSTRaw");
  inRaw->AddFile(fnameRaw);
  se->registerInputManager(inRaw);

  Fun4AllInputManager *inCalib = new Fun4AllDstInputManager("DSTCalib");
  inCalib->AddFile(fnameCalib);
  se->registerInputManager(inCalib);

  Process_Calo_Calib();
  
  caloHistGen *calo = new caloHistGen(outName);
  // What subsystems do you want?
  calo->doEMCal(1, "TOWERINFO_CALIB_CEMC");
  // Store EMCal clusters?
  calo->doClusters(1, "CLUSTERINFO_CEMC");

  // Store HCal information?
  calo->doHCals(1, "TOWERINFO_CALIB_HCALOUT", "TOWERINFO_CALIB_HCALIN");

  // Store ZDC information?
  calo->doZDC(1, "TOWERINFO_CALIB_ZDC");

  // Store GL1 Information?
  calo->doTrig(1, "GL1Packet");

  //reconstruct diphoton pairs?
  calo->setPi0Reco(1);
  calo->setMaxAlpha(0.7);
  calo->setMinClusterE(0.5);

  calo->setTrig("jet");
  calo->setTrig("photon");
  
  se->registerSubsystem(calo);

//   CaloOnly *calojy = new CaloOnly("CaloOnly", outNamejy);
//   se->registerSubsystem(calojy);

    se->skip(100);
  se->run(nEvents);
  se->End();
  se->PrintTimer();
  std::cout << "All done!" << std::endl;

  gSystem->Exit(0);
}

#endif