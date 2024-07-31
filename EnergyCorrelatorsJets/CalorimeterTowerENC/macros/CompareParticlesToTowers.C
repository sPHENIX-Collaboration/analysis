#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDSTInputManager.h>
#include <fun4all/SubsysReco.h>
#include <calorimetertowerenc/CalorimeterTowerENC.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libCalorimeterTowerENC.so)

int CompareParticlesToTowers(std::string truthjetfile, std::string calotowersfile, std::string truthrecofile, std::string globalrecofile, std::string n_evts)
{
	std::cout<<"Input truth jet: " <<truthjet <<"\n Input calo towers: " <<calotowers <<"\n Input truth reco: " <<truthreco <<"\n Input Global reco: " 
			<<globalreco <<"\n Run for " <<n_evts <<" events" <<std::endl;
	Global_Reco(); 
	Fun4AllServer* se=Fun4AllServer::instance();
	Fun4AllDSTInputManager *truthjet   = new Fun4AllDSTInputManager("truthjet");
	Fun4AllDSTInputManager *calotower  = new Fun4AllDSTInputManager("calotower");
	Fun4AllDSTInputManager *globalreco = new Fun4AllDSTInputManager("globalreco");
	Fun4AllDSTInputManager *truthreco  = new Fun4AllDSTInputManager("truthreco");
	std::vector<std::pari<std::string, Fun4AllDSTInputManager*> inputdst {{truthjetfile, truthjet}, {calotowersfile, calotowers}, {truthrecofile, truthreco}, {globalrecofile, globalreco}};
	int n_evts=std::stoi(n_evts), run_number=0, DST_Segment=0;
	for(auto input:inputdst)
	{
		std::stringstream filename (input.first);
		std::string runf="", segnf="", substr;
		if(run_number == 0){
			while(std::getline(filename, substr, "-")){
				if(run_number >= 0){
					runf=substr;
					try{
						run_number=std::stoi(runf);
					}
					catch(std::exception& e){ run_number = -1; }
				}
				else if(DST_Segement >= 0){
					segnf=substr;
					try{
						DST_Segment=std::stoi(segnf);
					}
					catch(std::exception& e ) { DST_Segment = -1; }
				}
			}
		}
		if(run_number < 0 || DST_Segment < 0 ){
			run_number = 0;
			DST_Segment = 0;
		}
		try{
			input.second->AddFile(input.first);
			se->registerInputManager(input.second);
		}
		catch(std::exception& e) {}
	}
	CalorimeterTowerENC* CalEval=new CalorimeterTowerENC(run_number, DST_Segment);
	se->RegisterSubsystem(CalEval);
	se->Run(n_evts);
	se->Print();
	return 1;
}
