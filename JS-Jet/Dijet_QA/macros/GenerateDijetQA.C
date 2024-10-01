//This is a test macro to try building the Dijet QA TTree before handing off to plots 
#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include "DijetQA.h"
#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <phhepmc/Fun4AllHepMCInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputPoolManager.h>
#include <fun4all/SubsysReco.h>
#include <phool/PHRandomSeed.h>
#include <phpythia8/PHPy8JetTrigger.h>
#include <sstream>
#include <string.h>
#include <G4_Global.C>
#include <algorithm>
#include <fstream> 
R__LOAD_LIBRARY(libfun4all.so);
R__LOAD_LIBRARY(libfun4allraw.so);
R__LOAD_LIBRARY(libDijetQA.so);
R__LOAD_LIBRARY(libffamodules.so);
R__LOAD_LIBRARY(libffarawmodules.so);
R__LOAD_LIBRARY(libphhepmc.so);

int GenerateDijetQA(std::string filename="")
{
	SetsPhenixStyle();
	Fun4AllServer* se=Fun4AllServer::instance();
	Fun4AllDstInputManager *in=new Fun4AllDstInputManager("in");
	in->fileopen(filename);
	DijetQA* dqa=new DijetQA();
	se->registerInputManager(in);
	se->registerSubsystem(dqa);
	se->run();
	dqa->Print();
	return 0;
}	
#endif
