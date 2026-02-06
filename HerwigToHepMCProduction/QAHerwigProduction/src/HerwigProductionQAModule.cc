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
		pythia 	= true;
	else 
		no_gen 	= true; //catch for an issue of having no generator, on first event immediately fail 
	this->verbosity	= verb;

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
int HerwigProductionQAModule::process_herwig_event(PHCompositeNode* topNode){
	//process data with a HepMC input 
	std::vector<std::vector<Jet*>*> identified_jets {}; //to hold r=0.2-r=0.6 jets 
	std::vector<HepMC::GenParticle*> photons {};
	std::vector<HepMC::GenParticle*> event_particles {};
	std::array<float,3> vertex {};
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
						if(!iter) continue;
						auto particle=(*iter);
						if(!particle) continue;
						if(particle->status==1 && particle->end_vertex() == false)
						{
							event_particles.push_back(particle);
							if( particle->pdg_id() == 22)
							{
								photons.push_back(photons);
							}
						}
					}
				}
			}
		}
	}
	findJets(event_particles, &identified_jets);
	if(photons) runAnalysisPhotonJets(identified_jets, photons, event_particles);
	else if (jets) runAnalysisJets(identified_jets, event_particles);
	return Fun4AllReturnCodes::EVENT_OK;

}
void HerwigProductionQAModule::findJets(std::vector<HepMC::GenParticle*> event_particles, std::vector<std::vector<Jet*>*>* jets)
{
	//just invoke fastJet over the relevant event particles and get some sembelance of truth jets at the HepMC level
	std::vector<fastjet::PseudoJet> candidates;
	fastjet::JetDefinition fjd2 (fastjet::antikt_alorithm, 0.2)
	fastjet::JetDefinition fjd3 (fastjet::antikt_alorithm, 0.3);
	fastjet::JetDefinition fjd4 (fastjet::antikt_alorithm, 0.4);
	fastjet::JetDefinition fjd5 (fastjet::antikt_alorithm, 0.5);
	fastjet::JetDefinition fjd6 (fastjet::antikt_alorithm, 0.6);
	for(auto p:event_particles)
	{
		auto pid = p->pdg_id();
		if (abs(pid) > 11 &&  abs(pid) < 19 ) continue;
		auto mom = p->momentum();
		candidates.push_back(fastjet::PseudoJet(mom.px(), mom.py(), mom.pz(), mom.E()));
	}	
	fastjet::ClusterSequence cs2(candidates, fjd2);
	fastjet::ClusterSequence cs3(candidates, fjd3);
	fastjet::ClusterSequence cs4(candidates, fjd4);
	fastjet::ClusterSequence cs5(candidates, fjd5);
	fastjet::ClusterSequence cs6(candidates, fjd6);
	auto js2=cs2.inclusive_jets();
	auto js3=cs3.inclusive_jets();
	auto js4=cs4.inclusive_jets();
	auto js5=cs5.inclusive_jets();
	auto js6=cs6.inclusive_jets();
	std::vector< std::vector<fastjet::PseudoJet>  > jets_size
	{
		std::make_pair(cs2, js2),
		std::make_pair(cs3, js3),
		std::make_pair(cs4, js4),
		std::make_pair(cs5, js5),
		std::make_pair(cs6, js6)
	};
	for(auto js:jets_size)
	{
		std::vector<Jet*>* jt=new std::vector<Jet*>();
		for(auto j:js)
		{
			Jet* jet = new Jet();
			jet->set_px(j.px());
			jet->set_py(j.py());
			jet->set_pz(j.pz());
			jet->set_e( j.e() );
			for(auto cmp:j.consituents())
			{
				jet->insert_comp(Jet::SRC::HEPMC_IMPORT, cmp.user_index());
			}
			jt->push_back(jet);
		}
		jets->push_back(jt);
	}
	return;
		
}

int HerwigProductionQAModule::process_pythia_event(PHCompositeNode* topNode){
	//just have to extract the HepMC input part of the DST
	//also grab the jets
	std::vector<Jet*> identified_jets;
	std::vector<HepMC::GenParticle*> photons;
	std::vector<HepMC::GenParticle*> event_particles;

	auto hepmc_gen_event= findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
	if(!hepmc_gen_event) return Fun4AllReturnCodes::EVENT_OK; 
	else if(hepmc_gen_event)
	{
		for(PHHepMCGenEventMap::ConstIter evtIter=hepmc_gen_event->begin(); evtIter != hepmc_gen_event->end(); ++evtIter)
		{
			PHHepMCGenEvent* hepev=evtIter->second;
			if(!hepev) continue;
			else if(hepev){
				HepMC::GenEvent* ev= hepev->getEvent();
				if( !ev ) continue;
				else if( ev )
				{
					for(HepMC::GenEvent::particle_const_iterator iter=ev->particles_begin(); iter != ev->particles_end(); ++iter) 
					{
						if(!iter) continue;
						auto particle=(*iter);
						if(!particle) continue;
						if(particle->status==1 && particle->end_vertex() == false)
						{
							event_particles.push_back(particle);
							if( particle->pdg_id() == 22)
							{
								photons.push_back(photons);
							}
						}
					}
				}
			}
		}
	}
	auto js2=findNode::getClass<JetContainerv1>(topNode, "AntiKt_Truth_r02");
	auto js3=findNode::getClass<JetContainerv1>(topNode, "AntiKt_Truth_r03");
	auto js4=findNode::getClass<JetContainerv1>(topNode, "AntiKt_Truth_r04");
	auto js5=findNode::getClass<JetContainerv1>(topNode, "AntiKt_Truth_r05");
	auto js6=findNode::getClass<JetContainerv1>(topNode, "AntiKt_Truth_r06");
	std::vector<JetContainerv1*> jts {js2, js3, js4, js5, js6};
	for(auto js:jts)
	{
		if(!js) continue;
		else if(js)
		{
			std::vector<Jets*> jetsize {};
			for(auto j:*js)
			{
				if(!j) continue;
				else if (j) 
				{
					jetsize.push_back(j);
				}
			}
			jets.push_back(&jetsize);
		}
	}
	if(photons) 		runAnalysisPhotonJets(identified_jets, photons);
	else if (jets)		runAnalysisJets(identified_jets);
	if( photons || jets ) 	runAnalysisEvent(event_particles);	
	return Fun4AllReturnCodes::EVENT_OK;
}

int HerwigProductionQAModule::runAnalysisJets(std::vector<std::vector<Jet*>*> jets_of_all_sizes)
{
	//run analysis of jets 
	int i=0;
	for(auto jets:jets_of_all_sizes)
	{
		float lead_pt	= 0.; 
		float lead_eta	= 0.;
		float lead_phi 	= 0.;
		float lead_e	= 0.;
		int lead_comp	= 0 ;
		for(auto jet:*jets){
			if(jet->get_pt()  > lead_pt){
			       	lead_pt 	= jet->get_pt();
				lead_eta	= jet->get_eta();
				lead_phi 	= jet->get_phi();
				lead_E 		= jet->get_e();
				lead_comp 	= (int)((jet->get_comp_vec()).size());
			}
			h_all_jets_pt->at(i)->	Fill(jet->get_pt());
			h_all_jets_eta->at(i)->	Fill(jet->get_eta());
			h_all_jets_phi->at(i)->	Fill(jet->get_phi());
			h_all_jets_e->at(i)->	Fill(jet->get_e());
			h_all_jets_n_comp->at(i)->Fill((int)((jet->get_comp_vec()).size()));
		}
		h_lead_jets_pt->at(i)->	Fill(lead_pt);
		h_lead_jets_eta->at(i)->Fill(lead_eta);
		h_lead_jets_phi->at(i)->Fill(lead_phi);
		h_lead_jets_e->at(i)->	Fill(lead_e);
		h_n->at(i)->Fill((int)(jets->size()));
		h_lead_n_comp->at(i)->	Fill(lead_comp);
		i++;
	}
	return Fun4AllReturnCodes::EVENT_OK;
}

int HerwigProductionQAModule::runAnalysisPhotonJets(std::vector<std::vector<Jet*>*> jets_of_all_sizes, std::vector<HepMC::GenParticle*> photons, std::vector<HepMC::GenParticle*> event)
{
	//run the analysis of the photons + jets
	runAnalysisJets(jets_of_all_sizes);
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
