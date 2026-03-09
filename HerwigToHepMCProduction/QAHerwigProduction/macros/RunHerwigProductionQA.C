#pragma once 
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)

#include <herwigproductionqamodule/HerwigProductionQAModule.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/SubsysReco.h>
#include <phhepmc/Fun4AllHepMCInputManager.h>
#include <string> 
#include <format>

R__LOAD_LIBRARY(libfun4all.so);
R__LOAD_LIBRARY(libffamodules.so);
R__LOAD_LIBRARY(libffarawmodules.so);
R__LOAD_LIBRARY(libphhepmc.so);
R__LOAD_LIBRARY(libHerwigProductionQAModule.so);


void RunHerwigProductionQA(int segment, std::string trigger_string, std::string generator, std::string file1, std::string file2="null")
{
	Fun4AllServer* se =Fun4AllServer::instance();
	std::string output_file = std::format("{0}_{1}-{2:06d}.root", generator, trigger_string, segment);
	std::cout<<output_file<<std::endl;
	if(file2.find("null") != std::string::npos)
	{
		Fun4AllHepMCInputManager *in	= new Fun4AllHepMCInputManager("in");
		se->registerInputManager(in);
		in->fileopen(file1);
	}
	else{
		Fun4AllDstInputManager *in_truth	= new Fun4AllDstInputManager("in_truth");
		Fun4AllDstInputManager *in_jet	= new Fun4AllDstInputManager("in_jet");
		se->registerInputManager(in_truth);
		se->registerInputManager(in_jet);
		in_truth->fileopen(file1);
		in_jet->fileopen(file2);
	}
	HerwigProductionQAModule* qa 	= new HerwigProductionQAModule(generator+trigger_string, output_file);
	se->registerSubsystem(qa);
	se->run();
	qa->Print();
	return;
};
#endif
