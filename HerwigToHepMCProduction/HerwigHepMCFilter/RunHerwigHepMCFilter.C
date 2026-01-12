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
#include <fun4allraw/Fun4AllPrdfInputManager.h>
#include <fun4allraw/Fun4AllPrdfInputPoolManager.h>
#include <fun4all/Fun4AllDstOutputManager.h>
#include <phhepmc/Fun4AllHepMCOutputManager.h>
#include <fun4all/SubsysReco.h>
#include <phool/PHRandomSeed.h>
#include <sstream>
#include <string>
#include <G4_Global.C>
#include <algorithm>
#include <fstream> 

R__LOAD_LIBRARY(libfun4all.so);
R__LOAD_LIBRARY(libfun4allraw.so);
R__LOAD_LIBRARY(libHepMCJetTrigger.so);
R__LOAD_LIBRARY(libffamodules.so);
R__LOAD_LIBRARY(libffarawmodules.so);
R__LOAD_LIBRARY(libphhepmc.so);

int RunHerwigHepMCFilter(std::string filename="/sphenix/user/sgross/sphenix_herwig/herwig_files/sphenix_10GeV_jetpt.hepmc", std::string trigger_type="jet", std::string trig="10", int goal_event_number=1000)
{
	float threshold=0.;
	try{
		threshold=std::stof(trig);
	}
	catch(std::exception& e){threshold=10.;}
	Fun4AllServer* se=Fun4AllServer::instance();
	std::string outfile=filename;
	outfile.insert(filename.find("-"),"_filtered");
	Fun4AllHepMCInputManager *in =new Fun4AllHepMCInputManager("in");
	Fun4AllHepMCOutputManager *out=new Fun4AllHepMCOutputManager("out", outfile);
	HepMCJetTrigger* hf=new HepMCJetTrigger(threshold, goal_event_number, true);
	HepMCParticleTrigger* part = new HepMCParticleTrigger(threshold, goal_event_number, true);
	if(trigger_type.find("photon") !=std::string::npos){
		part->AddParticles(22);
	}
	//std::fstream f;
	//f.open(filename);
	se->registerInputManager(in);
	in->fileopen(filename);
	se->registerOutputManager(out);
	if(trigger_type.find("jet") != std::string::npos) se->registerSubsystem(hf);
	if(trigger_type.find("photon") != std::string::npos) se->registerSubsystem(part);
	se->run();
	se->End();
	std::cout<<"Ran over " <<hf->n_evts <<" and found " <<hf->n_good <<" events" <<std::endl;
	return 0;
}	
#endif
