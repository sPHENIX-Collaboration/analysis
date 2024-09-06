#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/SubsysReco.h>
#include <calorimetertowerenc/CalorimeterTowerENC.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libCalorimeterTowerENC.so)

int CompareParticlesToTowers(std::string truthjetfile, std::string calotowersfile, std::string truthrecofile, std::string globalrecofile, std::string truthhitfile, std::string segments, std::string n_evt)
{
	std::cout<<"Input truth jet: " <<truthjetfile <<"\n Input calo towers: " <<calotowersfile <<"\n Input truth reco: " <<truthrecofile <<"\n Input Global reco: " 
			<<globalrecofile <<" Input truth hit file: " <<truthhitfile <<"\n Run for " <<n_evt <<" events" <<std::endl;
//	Global_Reco(); 
	Fun4AllServer* se=Fun4AllServer::instance();
	Fun4AllDstInputManager *truthjet   = new Fun4AllDstInputManager("truthjet");
	Fun4AllDstInputManager *calotower  = new Fun4AllDstInputManager("calotower");
	Fun4AllDstInputManager *globalreco = new Fun4AllDstInputManager("globalreco");
	Fun4AllDstInputManager *truthreco  = new Fun4AllDstInputManager("truthreco");
	Fun4AllDstInputManager *truthhit  = new Fun4AllDstInputManager("truthhit");
	std::vector<std::pair<std::string, Fun4AllDstInputManager*>> inputdst {{truthjetfile, truthjet}, {calotowersfile, calotower}, {truthrecofile, truthreco}, {globalrecofile, globalreco}, {truthhitfile, truthhit}};
	int n_evts=std::stoi(n_evt), run_number=15, DST_Segment=std::stoi(segments);
	for(auto input:inputdst)
	{
		std::stringstream filename (input.first);
		std::string runf="", segnf="", substr;
		if(run_number == 0){
			while(std::getline(filename, substr, '-')){
				if(run_number >= 0){
					runf=substr;
					try{
						run_number=std::stoi(runf);
					}
					catch(std::exception& e){ run_number = -1; }
				}
				else if(DST_Segment >= 0){
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
	std::cout<<"The Calorimeter tower has output file name " <<CalEval->outfilename <<std::endl;
	se->registerSubsystem(CalEval);
	se->run(n_evts);
	CalEval->Print();
	return 0;
}
#endif
