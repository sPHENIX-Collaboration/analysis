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
#include <fun4all/SubsysReco.h>
#include <ffamodules/CDBInterface.h>
#include <Calo_Calib.C>
#include <largerlenc/LargeRLENC.h>
#include <jetbase/FastJetAlgo.h>
#include <jetbase/JetReco.h>
#include <jetbase/TowerJetInput.h>
#include <jetbackground/RetowerCEMC.h>
#include <TFile.h>
#include <TTree.h>
#include <fstream>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libLargeRLENC.so)
R__LOAD_LIBRARY(libjetbase.so)
R__LOAD_LIBRARY(libjetbackground.so)

int RunLargeRLENC(std::string data_dst="none", std::string truthjetfile="none", std::string calotowersfile="none", std::string truthrecofile="none", std::string globalrecofile="none", std::string n_evt="0", std::string minpt="1.0", const std::string& dbtag="ProdA_2024")
{
	std::cout<<"actually processing this thing" <<std::endl;
	std::map<std::string, std::string> input_files {{data_dst, "data"}, {truthjetfile, "truthjet"}, {calotowersfile, "calotowers"}, {truthrecofile, "truthreco"}, {globalrecofile, "globalreco"}};
	Fun4AllServer* se=Fun4AllServer::instance();
	std::cout<<"Initiated the fun for all server" <<std::endl;
	int run_number=0, segment=0, n_evts=std::stoi(n_evt);
//	std::string run_str="", segn_str="", substr="";
	bool data=true;
	if(data_dst.find("none") != std::string::npos) data=false;
	for(auto f:input_files)
	{
		std::cout<<"working on the file for: " <<f.second <<std::endl;
		if(f.first.find("none") != std::string::npos) continue;
		std::cout<<f.first<<std::endl;
		std::stringstream filename(f.first);
		if(run_number == 0){
			std::cout<<"trying to get the run number" <<std::endl;
			std::pair<int, int> runseg=Fun4AllUtils::GetRunSegment(f.first);
			run_number=runseg.first;
			segment=runseg.second;
			std::cout<<"run " <<run_number <<" segement " <<segment <<std::endl;
		}
/*			while(std::getline(filename, substr, '-')){
				if(run_number==0){
					run_str=substr;
					try{ run_number=std::stoi(run_str);}
					catch(std::exception& e){ run_number=-1;}
				}
				if(run_number > 0){
					segn_str=substr;
					try{ segment=std::stoi(segn_str);}
					catch(std::exception& e){segment=-1;}
				}
			}
			if( run_number <=0 || segment <= 0 ) 
			{
				run_number = 0;
				segment = 0;
			}
		}*/ //turns out there was a one line for this apparently
		try{
			Fun4AllInputManager* input_file = new Fun4AllDstInputManager(f.second);
			input_file->AddFile(f.first);
			se->registerInputManager(input_file); 
		}
		catch(std::exception& e){std::cout<<"Unable to load file " <<f.first <<std::endl;}
	}
	recoConsts *rc = recoConsts::instance();
	rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
	rc->set_uint64Flag("TIMESTAMP", run_number);
	CDBInterface::instance() -> Verbosity(1);
	Process_Calo_Calib();
	bool nojets=true, retower_needed=true;
	if(data){ //check if the jet objects have already been constructed and retowering needed
		TFile* f1=new TFile(data_dst.c_str(), "READ");
		if(f1->IsOpen())
		{
			TTree* T=(TTree*) f1->Get("T");
			auto brlist=T->GetListOfBranches();
			for(int b=0; b<(int) brlist->Entries(); b++)
			{
				std::string branch_name (brlist[b]->GetName());
				std::transform(branch_name.begin, branch_name.end(), std::tolower);
				if(branch_name.find("retower")!= std::string::npos) retower_needed=false;
				if(branch_name.find("antikt")!= std::string::npos) nojets=false;
			}
		}
	}
	if(data && retower_needed){
		RetowerCEMC* rtcemc=new RetowerCEMC();
		rtcemc->Verbosity(0);
		rtcemc->set_tower_info(true);
		rtcemc->set_frac_cut(0.5);
		se->RegisterSubsystem(rtcemc);
	}
	if(data && nojets){ //if no jet objects, run fastjet
		JetReco* data_jets=new JetReco();
		data_jets->add_input(new TowerJetInput(Jet::CEMC_TOWERINFO_RETOWER);
		data_jets->add_input(new TowerJetInput(Jet::HCALIN_TOWERINFO);
		data_jets->add_input(new TowerJetInput(Jet::HCALOUT_TOWERINFO);
	i	data_jets->add_algo(new FastJetAlgoSub(Jet::ANTIKT, 0.4), "AntiKt_TowerInfo_r04");
		data_jets->set_algo_node("ANTIKT");
		data_jets->set_input_node("TOWER");
		data_jets->Verbosity(0);
		se->registerSubsystem(data_jets);
		//no background subtracting as these won't be used for real analysis, just to provide rough cuts
		}
	std::cout<<"Loaded all subparts in, now loading in the analysis code" <<std::endl;
//	std::string text_out_filename="/gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/LargeRLCaloENC/Missing_pT_for_felix_run-"+std::to_string(run_number)+"-"+std::to_string(segment)+".csv";
	//std::fstream* ofs=new std::fstream(text_out_filename);
	LargeRLENC* rlenc=new LargeRLENC(run_number, segment, std::stof(minpt), data);
	se->registerSubsystem(rlenc);
	std::cout<<"Runing for " <<n_evt <<" events" <<std::endl;
	se->run(n_evts);
	rlenc->Print();
	return 0;
}
#endif		
