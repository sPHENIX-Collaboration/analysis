#pragma once
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,00,0)
#include <hepmcjettrigger/HepMCJetTrigger.h>
#include <hepmcjettrigger/HepMCParticleTrigger.h>
#include "sPhenixStyle.h"
#include "sPhenixStyle.C"
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <phhepmc/Fun4AllHepMCInputManager.h>
//#include <fun4all/Fun4AllDstInputManager.h>
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
		part->AddParticles(22);
	}
	se->registerInputManager(in);
	in->fileopen(filename);
	se->registerOutputManager(out);
	if(trigger_type.find("jet") != std::string::npos) se->registerSubsystem(hf);
	if(trigger_type.find("photon") != std::string::npos) se->registerSubsystem(part);
	se->run();
 	se->End();
	if(trigger_type.find("jet") != std::string::npos ) std::cout<<"Ran over " <<hf->n_evts <<" and found " <<hf->n_good <<" events" <<std::endl;
	if(trigger_type.find("photon") != std::string::npos ) std::cout<<"Ran over " <<part->n_evts<<" and found " <<part->n_good <<" events" <<std::endl;
	if(segment == 0 ) {
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
						xs_val=std::stof(subline.substr(0, left_par-1));
						auto pow_e = subline.find("e");
						xs_pow=std::stof(subline.substr(pow_e+2));
					}
				}
			}
		}
		float xs=0.;
		if(trigger_type.find("jets") != std::string::npos) xs = xs_value * std::pow(10, xs_pow) * ((float)hf->n_good)/((float)hf->n_evts*(float)nGen);
		if(trigger_type.find("photon") != std::string::npos) xs = xs_value * std::pow(10, xs_pow) * ((float)part->n_good)/((float)part->n_evts*(float)nGen);
		f.close();
		std::fstream of;
		std::string xs_of="Cross_Section_"+trigger_type+trig+".txt";
		of.open(xs_of.c_str());
		if(trigger_type.find("jets") != std::string::npos) of<<"Events analyzed= " <<hf->n_evts <<", Events passing filter= " <<hf->n_good <<", Total Generated events= " <<nGen <<", XS/N= "<<xs<<std::endl;
		if(trigger_type.find("photons") != std::string::npos) of<<"Events analyzed= " <<part->n_evts <<", Events passing filter= " <<part->n_good <<", Total Generated events= " <<nGen <<", XS/N= "<<xs <<std::endl;
		of.close();

	}
	
	return 0;
}	
#endif
