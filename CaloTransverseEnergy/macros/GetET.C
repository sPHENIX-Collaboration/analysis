#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <calotransverseenergy/CaloTransverseEnergy.h>
#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputPoolManager.h>
#include <fun4allraw/SinglePrdfInput.h>
#include <fun4all/SubsysReco.h>
#include <sstream>
#include <string.h>
#include <G4_Global.C>

R__LOAD_LIBRARY(libfun4all.so);
R__LOAD_LIBRARY(libfun4allraw.so);
R__LOAD_LIBRARY(libcalo_io.so);
R__LOAD_LIBRARY(libCaloTransverseEnergy.so);
R__LOAD_LIBRARY(libffamodules.so);
R__LOAD_LIBRARY(libffarawmodules.so);

int GetET(std::string filename="/sphenix/tg/tg01/jets/ahodges/run23_production/21518/DST-00021518-0000.root")
{
	//taking in a data file and will run over all data 
	//std::cout<<"input of " <<argc <<std::endl;
	//std::string filename (argv[1]);
//	std::string filename="/sphenix/tg/tg01/jets/ahodges/run23_production/21518/DST-00021518-0000.root";
	std::cout<<"Input file is " <<filename <<std::endl;
	SetsPhenixStyle();
	Fun4AllServer *se=Fun4AllServer::instance();
	Fun4AllPrdfInputPoolManager *inp = new Fun4AllPrdfInputPoolManager("inp");
	Fun4AllDstInputManager *in = new Fun4AllDstInputManager("in");
	//in->AddFile(filename);
	CaloTransverseEnergy* cte=new CaloTransverseEnergy(filename, "CaloET");
	std::stringstream subs(filename);
	std::string substr;
	int is_input=0;
	//load in the DST Segment and run number
	while(std::getline(subs, substr, '-')){
		if(is_input==2){
			std::stringstream sss(substr);
			std::string subsub;
			while(std::getline(sss, subsub, '.')){
			try{
			 	cte->DST_Segment=std::stoi(substr);
				}	
			catch(std::exception* e) {}
			}
		} 
		if(is_input==1){
			std::cout<<substr<<std::endl; 
			cte->run_number=std::stoi(substr);
			is_input=2;
		}
		if(substr.find("DST")!=std::string::npos) is_input=1;
		if(substr.find("run")!=std::string::npos) is_input=1; 
	}
	std::cout<<"Have loaded in the subsystem" <<std::endl;
	if(filename.find("DST") != std::string::npos){
		in->AddFile(filename); 
		se->registerInputManager(in);
	}
	if(filename.find("prdf") != std::string::npos){
		inp->AddPrdfInputList(filename)->MakeReference(true);
		se->registerInputManager(inp);
		cte->isPRDF=true;
	}
	se->registerSubsystem(cte);
	se->run();
	std::cout<<"Writing to file"<<std::endl;
        cte->ProduceOutput();
	return 0;				
}
#endif	
