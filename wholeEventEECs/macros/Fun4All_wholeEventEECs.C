#include <string>
#include <iostream>
#include <format>

#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/SubsysReco.h>
#include <ffamodules/CDBInterface.h>
#include <Calo_Calib.C>

#include <wholeeventeecs/wholeEventEECs.h>
#include <jetbackground/RetowerCEMC.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libwholeEventEECs.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libjetbackground.so)

void Fun4All_wholeEventEECs(const std::string caloDST = "/sphenix/lustre01/sphnxpro/production2/run2pp/physics/jetskimmer/ana509_2024p022_v001/run_00047200_00047300/DST_JETCALO_run2pp_ana509_2024p022_v001-00047289-00000.root", const std::string jetDST = "/sphenix/lustre01/sphnxpro/production2/run2pp/physics/jetskimmer/ana509_2024p022_v001/run_00047200_00047300/DST_Jet_run2pp_ana509_2024p022_v001-00047289-00000.root", const std::string outDir = "/sphenix/tg/tg01/jets/bkimelman/wEEC/")
{
  Fun4AllServer* se=Fun4AllServer::instance();
  se->Verbosity(0);

  std::pair<int,int> runseg = Fun4AllUtils::GetRunSegment(caloDST);
  int runnumber = runseg.first;
  int segment = runseg.second;

  Fun4AllInputManager *inCalo = new Fun4AllDstInputManager("InputManagerCalo");
  inCalo->AddFile(caloDST);
  se->registerInputManager(inCalo);
  
  Fun4AllInputManager *inJet = new Fun4AllDstInputManager("InputManagerJet");
  inJet->AddFile(jetDST);
  se->registerInputManager(inJet);
  
  auto rc = recoConsts::instance();
  rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
  rc->set_uint64Flag("TIMESTAMP", runnumber);
  CDBInterface::instance() -> Verbosity(0);
  Process_Calo_Calib();

  /*
  RetowerCEMC* rtcemc=new RetowerCEMC("RetowerCEMC");
  rtcemc->Verbosity(0);
  rtcemc->set_towerinfo(true);
  rtcemc->set_frac_cut(0.5);
  se->registerSubsystem(rtcemc);
  */

  wholeEventEECs *wEEC = new wholeEventEECs("wholeEventEECs");
  wEEC->set_outputName(std::format("{}/wholeEventEECs_run2pp_ana509_2024p022_v001-{}-{}.root",outDir, runnumber, segment).c_str());
  se->registerSubsystem(wEEC);

  se->run(-1);
  se->End();
  se->PrintTimer();

  delete se;
  gSystem->Exit(0);
}
