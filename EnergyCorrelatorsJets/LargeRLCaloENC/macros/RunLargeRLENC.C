#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <string>
#include <sstream>
#include <vector>
#include <utility>
#include <map>
#include <fun4all/Fun4AllBase.h>
#include <fun4all/Fun4AllUtils.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
#include <fun4all/SubsysReco.h>
#include <Calo_Calib.C>
#include <largerlenc/LargeRLENC.h>

R__LOAD_LIBRARY(libfun4all.so)
R__LOAD_LIBRARY(libcalo_io.so)
R__LOAD_LIBRARY(libffamodules.so)
R__LOAD_LIBRARY(libLargeRLENC.so)

int RunLargeRLENC(std::string data_dst="none", std::string truthjetfile="none", std::string calotowersfile="none", std::string truthrecofile="none", std::string globalrecofile="none", std::string n_evt="0", std::string minpt="1.0")
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
/*	recoConsts *rc = recoConsts::instance();
	rc->set_StringFlag("CDB_GLOBALTAG", dbtag);
	rc->set_uint64Flag("TIMESTAMP", run_number);
	CDBInterface::instance() -> Verbosity(1);*/
	std::cout<<"Loaded all subparts in, now loading in the analysis code" <<std::endl;
	LargeRLENC* rlenc=new LargeRLENC(run_number, segment, std::stof(minpt), data);
	se->registerSubsystem(rlenc);
	std::cout<<"Runing for " <<n_evt <<" events" <<std::endl;
	se->run(n_evts);
	rlenc->Print();
	return 0;
}
#endif		
