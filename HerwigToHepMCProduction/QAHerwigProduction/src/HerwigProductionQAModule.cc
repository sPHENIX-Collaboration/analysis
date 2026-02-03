//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in HerwigProductionQAModule.h.
//
// HerwigProductionQAModule(const std::string &name = "HerwigProductionQAModule")
// everything is keyed to HerwigProductionQAModule, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// HerwigProductionQAModule::~HerwigProductionQAModule()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int HerwigProductionQAModule::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int HerwigProductionQAModule::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int HerwigProductionQAModule::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT; 
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT; 
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int HerwigProductionQAModule::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int HerwigProductionQAModule::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int HerwigProductionQAModule::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int HerwigProductionQAModule::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void HerwigProductionQAModule::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "HerwigProductionQAModule.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//____________________________________________________________________________..
HerwigProductionQAModule::HerwigProductionQAModule(const std::string data_type_label, float trigger, int verb, const std::string &name):
 SubsysReco(name)
{
	//A very basic module to check the kinematics of a Herwig production
	if( data_type_label.find("Photon") != std::string::npos || data_type_label.find("photon") != std::string::npos ) 
		photon 	= true;
	if( data_type_label.find("Jet") != std::string::npos || data_type_label.find("jet") != std::string::npos ) 
		jet 	= true;
	if( data_type_label.find("Herwig") != std::string::npos || data_type_label.find("herwig") != std::string::npos ) 
		herwig 	= true;
	else if ( data_type_label.find("Pythia") != std::string::npos || data_type_label.find("pythia") != std::string::npos) 
		pythia = true;
	else 
		no_gen = true; //catch for an issue of having no generator, on first event immediately fail 
	this->verbosity=verb;

}

//____________________________________________________________________________..
HerwigProductionQAModule::~HerwigProductionQAModule()
{
}

//____________________________________________________________________________..
int HerwigProductionQAModule::Init(PHCompositeNode *topNode)
{
	return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HerwigProductionQAModule::InitRun(PHCompositeNode *topNode)
{
	return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HerwigProductionQAModule::process_event(PHCompositeNode *topNode)
{
	if(no_gen) return Fun4AllReturnCodes::ABORT_RUN;
	n_evt++;
	if(verbosity >= 1 ) std::cout<<"Working on event " <<n_evt <<std::endl;
	if(herwig) process_herwig_event(topNode); //processes just a hepmc node
	if(pythia) process_pythia_event(topNode); //processes a fully reconstructed pythia sample
	
	return Fun4AllReturnCodes::EVENT_OK;
}
int process_herwig_event(PHCompositeNode* topNode){
	//process data with a HepMC input 
	std::vector<Jet*> identified_jets;
	std::vector<HepMC::GenParticle*> photons;
	std::vector<HepMC::GenParticle*> event_particles;
	std::array<float,3> vertex;
	PHHepmMCGenEventMap* phg=findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
	if(!phg){ 
		return 1; //catch empty event pmap
	}
	else{
		for(PHHepMCGenEventMap::ConstIter eventIter=phg->begin(); eventIter != phg->end(); ++eventIter){
			PHHepMCGenEvent* hepev=eventIter->second;
			if(!hepev){
				continue;
			}
			else{
				HepMC::GenEvent ev=hepev->getEvent();
				if(!ev) continue;
				else{
					auto vtx = ev->signal_process_vertex();
					if(!vtx) continue;
					else{ //fill in info from the generator vertex
						auto vtx_pos = vtx->position();
						vertex[0] = vtx_pos->x();
						vertex[1] = vtx_pos->y();
						vertex[2] = vtx_pos->z();
						h_vertex_herwig_x->Fill(vertex[0]);
						h_vertex_herwig_y->Fill(vertex[1]);
						h_vertex_herwig_z->Fill(vertex[2]);
						h_vertex_herwig_rz->Fill(std::sqrt(std::pow(vertex[0], 2) + std::pow(vertex[1] ,2)), vertex[3]);
						h_vertex_herwig_thetaz->Fill(std::atan2(vertex[1], vertex[0]), vertex[2]);
					}
					for(HepMC::GenEvent::particle_const_iterator iter=ev->particles_begin(); iter != ev->particles_end(); ++iter) 
					{



}
int process_pythia_event(PHCompositeNode* topNode){
	//just have to extract the HepMC input part of the DST
	//also grab the jets
	std::vector<Jet*> identified_jets;
	std::vector<HepMC::GenParticle*> photons;
	std::vector<HepMC::GenParticle*> event_particles;
	 
}

int runAnalysisJets(std::vector<Jet*> jets, std::vector<HepMC::GenParticle*> event)
{
	//a
}

int runAnalysisPhotonJets(std::vector<Jet*> jets, std::vector<HepMC::GenParticle*> photons, std::vector<HepMC::GenParticle*> event)
{
	//a
}
//____________________________________________________________________________..
int HerwigProductionQAModule::ResetEvent(PHCompositeNode *topNode)
{
	return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HerwigProductionQAModule::EndRun(const int runnumber)
{
	return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HerwigProductionQAModule::End(PHCompositeNode *topNode)
{
	return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HerwigProductionQAModule::Reset(PHCompositeNode *topNode)
{
	return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void HerwigProductionQAModule::Print(const std::string &what) const
{
	return;
}
