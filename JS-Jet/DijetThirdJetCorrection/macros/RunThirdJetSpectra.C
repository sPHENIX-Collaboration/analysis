
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
#include <phhepmc/Fun4AllHepMCInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/SubsysReco.h>
#include <ffamodules/CDBInterface.h>
#include <hepmcjettrigger/HepMCJetTrigger.h>
#include <thirdjetspectra/ThirdJetSpectra.h>


R__LOAD_LIBRARY(libfun4all.so);
R__LOAD_LIBRARY(libffamodules.so);
R__LOAD_LIBRARY(libHepMCJetTrigger.so);
R__LOAD_LIBRARY(libThirdJetSpectra.so);
R__LOAD_LIBRARY(libphhepmc.so);

int RunThirdJetSpectra(std::string filename, std::string segment)
{	
	//run jet spectra using some parameters 
	Fun4AllServer* se = Fun4AllServer::instance();
	Fun4AllHepMCInputManager* in = new Fun4AllHepMCInputManager("in");
	se->registerInputManager(in);
	se->fileopen(in->Name().c_str(), filename);
	ThirdJetSpectra* spec=new ThirdJetSpectra(segment);
	HepMCJetTrigger* trig=new HepMCJetTrigger(30.);
	se->registerSubsystem(trig);
	se->registerSubsystem(spec);
	se->run(); //for test purposes
	spec->Print();
	return 0;
}
#endif
