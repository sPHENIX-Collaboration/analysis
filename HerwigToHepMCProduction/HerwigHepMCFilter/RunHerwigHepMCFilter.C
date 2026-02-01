#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <hepmcjettrigger/HepMCJetTrigger.h>
#include <hepmcjettrigger/HepMCParticleTrigger.h>
#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <phhepmc/Fun4AllHepMCInputManager.h>
#include <fun4all/Fun4AllDstInputManager.h>
//#include <fun4allraw/Fun4AllPrdfInputManager.h>
//#include <fun4allraw/Fun4AllPrdfInputPoolManager.h>
//#include <fun4all/Fun4AllDstOutputManager.h>
#include <phhepmc/Fun4AllHepMCOutputManager.h>
#include <fun4all/SubsysReco.h>
#include <phool/PHRandomSeed.h>
#include <sstream>
#include <string>
#include <algorithm>
#include <fstream> 
#include <cmath>

R__LOAD_LIBRARY(libfun4all.so);
//R__LOAD_LIBRARY(libfun4allraw.so);
R__LOAD_LIBRARY(libHepMCJetTrigger.so);
R__LOAD_LIBRARY(libHepMCParticleTrigger.so);
R__LOAD_LIBRARY(libffamodules.so);
R__LOAD_LIBRARY(libffarawmodules.so);
R__LOAD_LIBRARY(libphhepmc.so);

int RunHerwigHepMCFilter(std::string filename="/sphenix/user/sgross/sphenix_herwig/herwig_files/sphenix_10GeV_jetpt.hepmc", std::string trigger_type="jet", std::string trig="10", int goal_event_number=1000, int nGen=1000000, std::string xs_file = "none")
{
	float threshold=0.;
	try{
		threshold=std::stof(trig);
	}
	catch(std::exception& e){threshold=10.;}
	std::cout<<"Trying this " <<std::endl;
	Fun4AllServer* se=Fun4AllServer::instance();
	std::string outfile=filename;
	int segment=-1; 
        auto first_break = filename.find("-");
	auto second_break = filename.find(".he");
	std::string seg = filename.substr(first_break + 1 , second_break - first_break -1 );
	try{
		segment = std::stoi(seg);
	}
	catch(std::exception& e){}
	outfile.insert(filename.find("-"),"_filtered");
	Fun4AllHepMCInputManager *in =new Fun4AllHepMCInputManager("in");
	Fun4AllHepMCOutputManager *out=new Fun4AllHepMCOutputManager("out", outfile);
	HepMCJetTrigger* hf=new HepMCJetTrigger(threshold, goal_event_number, true);
	HepMCParticleTrigger* part = new HepMCParticleTrigger(threshold, goal_event_number, true);
	if(trigger_type.find("photon") !=std::string::npos){
		part->AddParticle(22);
	}
	se->registerInputManager(in);
	in->fileopen(filename);
	se->registerOutputManager(out);
	if(trigger_type.find("jet") != std::string::npos) se->registerSubsystem(hf);
	if(trigger_type.find("photon") != std::string::npos) se->registerSubsystem(part);
	se->run();
 	se->End();
	int n_good, n_evts;

	if(trigger_type.find("jet") != std::string::npos ){
		n_evts = hf->getNevts();
		n_good=hf->getNgood();
	}

	if(trigger_type.find("photon") != std::string::npos ){
		n_evts = part->getNevts();
		n_good=part->getNgood();
	}
	std::cout<<"Analyzed events = " <<n_evts <<"\n Good events= " <<n_good <<std::endl;
	if(segment < 20 ) {
		std::fstream f;
		f.open(xs_file);
		float xs_value = 0.;
		float xs_pow = 0.;
		std::string line;
		while(std::getline(f, line))
		{
			if(line.find("generated events") != std::string::npos)
			{
				std::stringstream linestream (line);
				std::string subline;
				while(std::getline(linestream, subline, ' '))
				{
					if(subline.find(")e")!=std::string::npos)
					{
						auto left_par = subline.find("(");
						xs_value=std::stof(subline.substr(0, left_par-1));
						auto pow_e = subline.find("e");
						xs_pow=std::stof(subline.substr(pow_e+2));
					}
				}
			}
		}
		float xs = xs_value * std::pow(10, xs_pow) * ((float)n_good)/((float)n_evts*(float)nGen);
		f.close();
		std::ofstream of;
		std::string xs_of="cross_section_data/Cross_Section_"+trigger_type+trig+"_"+std::to_string(segment)+".txt";
		of.open(xs_of.c_str());
		if(!of.good()) std::cout<<"failed to open file" <<std::endl;
		if(of.good())
			of<<"Events analyzed= " << n_evts<<"\n Events passing filter= " <<n_good <<"\n Total Generated events= " <<nGen <<"\n XS/N= "<<xs 
			<<"\n Read xs = " <<xs_value <<" x 10^ " <<xs_pow <<" nb " <<std::endl;
		else 
			std::cout<<"Events analyzed= " << n_evts<<"\n Events passing filter= " <<n_good <<"\n Total Generated events= " <<nGen <<"\n XS/N= "<<xs 
			<<"\n Read xs = " <<xs_value <<" x 10^ " <<xs_pow <<" nb " <<std::endl;
		of.close();
		std::cout<<"Cross section / Evt = " <<xs<<std::endl;
	}
	
	return 0;
}	
#endif
