#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <map>
#include <algorithm>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/Fun4AllNoSyncDstInputManager.h>
#include <fun4all/SubsysReco.h>
#include <ffamodules/CDBInterface.h>
#include <Calo_Calib.C>
#include <dummy/dummy.h>
#include <largerlenc/LargeRLENC_LEDPedestalScan.h>
#include <largerlenc/LargeRLENC.h>
#include <jetbase/FastJetAlgo.h>
#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>
#include <jetbackground/RetowerCEMC.h>
#include <phool/recoConsts.h>
#include <TFile.h>
#include <TTree.h>
#include <fstream>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libLargeRLENC.so)
R__LOAD_LIBRARY(libdummy.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libjetbackground.so)

int RunLargeRLENC_LEDPedestalScan(int run_N, int segment, std::string hcal="none", std::string emcal="none", int n_evts=0, bool runE2C=false)
{
	Fun4AllServer* se=Fun4AllServer::instance();
	LEDPedestalScan* sc=new LEDPedestalScan(run_N, segment, runE2C);
	Fun4AllNoSyncDstInputManager* in1=new Fun4AllNoSyncDstInputManager("in_hcal");
	Fun4AllNoSyncDstInputManager* in2=new Fun4AllNoSyncDstInputManager("in_emcal");
	in1->AddFile(hcal);
	in2->AddFile(emcal);
	se->registerInputManager(in1);
	se->registerInputManager(in2);
	recoConsts* rc=recoConsts::instance();
	rc->set_StringFlag("CDB_GLOBALTAG", "ProdA_2024");
	rc->set_uint64Flag("TIMESTAMP", run_N);
	CDBInterface::instance()->Verbosity(0);
	se->Verbosity(0);
	if(run_N != 42959 && run_N !=49277) Process_Calo_Calib();
	se->registerSubsystem(sc);
	se->run(0);
	sc->Print();
	return 0;
}
#endif
