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
	this->trigger_val = trigger;
}

//____________________________________________________________________________..
HerwigProductionQAModule::~HerwigProductionQAModule()
{
}

//____________________________________________________________________________..
int HerwigProductionQAModule::Init(PHCompositeNode *topNode)
{
	if(verbosity > 1) std::cout<<"Trigger val: " <<trigger_val <<std::endl;
	if(jet)
	{
		for(int i=2; i<7; i++)
		{
			//all jets
			TH1F* h_all_jet_pt=new TH1F(Form("h_jet_r0%d_pt", i ), 
					Form(" R=0.%d jets; p_{T}^{jet}[GeV]; N_{jets}", i),
					500, -0.5, 99.5);
			TH1F* h_all_jet_eta=new TH1F(Form("h_jet_r0%d_eta", i ), 
					Form(" R=0.%d jets; #eta_{jet}; N_{jets}", i),
					100, -1.2, 1.2);
			TH1F* h_all_jet_phi=new TH1F(Form("h_jet_r0%d_phi", i ), 
					Form(" R=0.%d jets; #varphi_{jet}; N_{jets}", i),
					100, 0, 2*M_PI);
			TH1F* h_all_jet_e=new TH1F(Form("h_jet_r0%d_e", i ), 
					Form(" R=0.%d jets; E_{jet}[GeV]; N_{jets}", i),
					500, -0.5, 99.5);
			TH1I* h_all_jet_n_comp = new TH1I(Form("h_jet_r0%d_comp", i),
					Form(" R=0.%d jets; N_{comp}; N_{jets}", i),
					100, 0, 100);
			//add to the vector
			h_all_jets_pt.push_back(h_all_jet_pt);
			h_all_jets_eta.push_back(h_all_jet_eta);
			h_all_jets_phi.push_back(h_all_jet_phi);
			h_all_jets_e.push_back(h_all_jet_e);
			h_all_jets_n_comp.push_back(h_all_jet_n_comp);

			//leading jets
			TH1F* h_lead_jet_pt=new TH1F(Form("h_lead_jet_r0%d_pt", i ), 
					Form(" R=0.%d jets; p_{T}^{lead jet}[GeV]; N_{evts}", i),
					500, -0.5, 99.5);
			TH1F* h_lead_jet_eta=new TH1F(Form("h_lead_jet_r0%d_pt", i ), 
					Form(" R=0.%d jets; #eta_{jet}^{lead}; N_{evts}", i),
					100, -1.2, 1.2);
			TH1F* h_lead_jet_phi=new TH1F(Form("h_lead_jet_r0%d_pt", i ), 
					Form(" R=0.%d jets; #varphi_{jet}^{lead}; N_{evts}", i),
					100, 0, 2*M_PI);
			TH1F* h_lead_jet_e=new TH1F(Form("h_lead_jet_r0%d_pt", i ), 
					Form(" R=0.%d jets; E_{jet}^{lead}[GeV]; N_{evts}", i),
					500, -0.5, 99.5);
			TH1I* h_lead_jet_n_comp = new TH1I(Form("h_lead_jet_r0%d_comp", i),
					Form(" R=0.%d jets; N_{comp}^{lead jet}; N_{evts}", i),
					100, 0, 100);
			//add to the vector
			h_lead_jets_pt.push_back(h_lead_jet_pt);
			h_lead_jets_eta.push_back(h_lead_jet_eta);
			h_lead_jets_phi.push_back(h_lead_jet_phi);
			h_lead_jets_e.push_back(h_lead_jet_e);
			h_lead_jets_n_comp.push_back(h_lead_jet_n_comp);

			TH1I* h_n_jet = new TH1I(Form("h_jet_r0%d_n", i), 
					Form(" R=0.%d jets; N_{jet}; N_{event}", i),
					50, 0, 50);
			h_n_jets.push_back(h_n_jet);
		}
	}
	if(photon)
	{
		//all photons
		h_all_photons_pt=new TH1F("h_photons_pt", 
				"all  photons; p_{T}^{photon}[GeV]; N_{photons}",
				500, -0.5, 99.5);
		h_all_photons_eta=new TH1F("h_photons_eta", 
				"all  photons; #eta_{photon}; N_{photons}",
				100, -1.2, 1.2);
		h_all_photons_phi=new TH1F("h_photons_phi", 
				"all  photons; #varphi_{photon}; N_{photons}",
				100, 0, 2*M_PI);
		h_all_photons_e=new TH1F("h_photons_e", 
				"all  photons; E_{photon}[GeV]; N_{photons}",
				500, -0.5, 99.5);
		h_n_photons 	=new TH1I("h_n_photons",
				"all  photons; N_{photons}; N_{evts}",
				100, 0, 100);
		
		//leading photons
		h_lead_photons_pt=new TH1F("h_lead_photons_pt", 
				"lead  photons; p_{T}^{lead photon}[GeV]; N_{evts}",
				500, -0.5, 99.5);
		h_lead_photons_eta=new TH1F("h_lead_photons_eta", 
				"lead  photons; #eta_{photon}^{lead}; N_{evts}",
				100, -1.2, 1.2);
		h_lead_photons_phi=new TH1F("h_lead_photons_phi", 
				"lead  photons; #varphi_{photon}^{lead}; N_{evts}",
				100, 0, 2*M_PI);
		h_lead_photons_e=new TH1F("h_lead_photons_e", 
				"lead  photons; E_{photon}^{lead}[GeV]; N_{evts}",
				500, -0.5, 99.5);
		
		//direct photons
		h_direct_photons_pt=new TH1F("h_direct_photons_pt", 
				"direct  photons; p_{T}^{photon}[GeV]; N_{photon}",
				500, -0.5, 99.5);
		h_direct_photons_eta=new TH1F("h_direct_photons_eta", 
				"direct  photons; #eta_{photon}^{direct}; N_{photon}",
				100, -1.2, 1.2);
		h_direct_photons_phi=new TH1F("h_direct_photons_phi", 
				"direct  photons; #varphi_{photon}^{direct}; N_{photon}",
				100, 0, 2*M_PI);
		h_direct_photons_e=new TH1F("h_direct_photons_E", 
				"direct  photons; E_{photon}[GeV]; N_{photon}",
				500, -0.5, 99.5);
		h_n_direct 	=new TH1I("h_n_direct",
				"direct  photons; N_{photons}; N_{photon}",
				100, 0, 100);
		
		//fragmentation photons
		h_frag_photons_pt=new TH1F("h_frag_photons_pt", 
				"fragmentation photons; p_{T}^{photon}[GeV]; N_{photon}",
				500, -0.5, 99.5);
		h_frag_photons_eta=new TH1F("h_frag_photons_eta", 
				"fragmentation photons; #eta_{photon}^{frag}; N_{photon}",
				100, -1.2, 1.2);
		h_frag_photons_phi=new TH1F("h_frag_photons_phi", 
				"fragmentation photons; #varphi_{photon}^{frag}; N_{photon}",
				100, 0, 2*M_PI);
		h_frag_photons_e=new TH1F("h_frag_photons_E", 
				"fragmentation photons; E_{photon}[GeV]; N_{photon}",
				500, -0.5, 99.5);
		h_n_frag 	=new TH1I("h_n_frag",
				"fragmentation  photons; N_{photons}; N_{photon}",
				100, 0, 100);

	}
	if(photon && jet)
	{
		for(int i=2; i<7; i++)
		{
			//leading jet + photon
			TH2F* h_photon_jet_pt_a=new TH2F(Form("h_photon_jet_r0%d_pt", i ), 
					Form(" R=0.%d jets; p_{T}^{photon}[GeV]; p_{T}^{jet}[GeV]; N_{evts}", i),
					500, -0.5, 99.5, 500, -0.5, 99.5);
			TH2F* h_photon_jet_eta_a=new TH2F(Form("h_photon_jet_r0%d_eta", i ), 
					Form(" R=0.%d jets; #eta_{photon}; #eta_{jet}; N_{evts}", i),
					100, -1.2, 1.2, 100, -1.2, 1.2);
			TH2F* h_photon_jet_phi_a=new TH2F(Form("h_photon_jet_r0%d_phi", i ), 
					Form(" R=0.%d jets; #varphi_{photon}; #varphi_{jet}; N_{evts}", i),
					100, 0, 2*M_PI, 100, 0, 2*M_PI);
			TH2F* h_photon_jet_e_a=new TH2F(Form("h_photon_jet_r0%d_e", i ), 
					Form(" R=0.%d jets; E_{photon}[GeV]; E_{jet}; N_{evts}", i),
					500, -0.5, 99.5, 500, -0.5, 99.5);
			TH1F* h_photon_jet_dphi_a = new TH1F(Form("h_photon_jet_r0%d_dphi", i),
					Form(" R=0.%d jets; |#Delta #varphi|_{photon jet}; N_{evts}", i),
					100, -0.1, M_PI+0.1);
			//add to the vector
			h_photon_jet_pt.push_back(h_photon_jet_pt_a);
			h_photon_jet_eta.push_back(h_photon_jet_eta_a);
			h_photon_jet_phi.push_back(h_photon_jet_phi_a);
			h_photon_jet_e.push_back(h_photon_jet_e_a);
			h_photon_jet_dphi.push_back(h_photon_jet_dphi_a);
		}
	}
				
	//event categorization 
	//1D distributions
	h_particle_eta	= new TH1F("h_particle_eta" , "Final State Particles; #eta; N_{particles}", 100, -1.2, 1.2);
	h_particle_phi 	= new TH1F("h_particle_phi" , "Final State Particles; #varphi; N_{particles}", 100, 0, 2*M_PI);
	h_particle_e	= new TH1F("h_particle_e"   , "Final State Particles; E [GeV]; N_{particles}", 1000, -0.5, 99.5);
	h_particle_et	= new TH1F("h_particle_et"  , "Final State Particles; E_{T} [GeV]; N_{particles}", 1000, -0.5, 99.5);
	h_particle_pt	= new TH1F("h_particle_pt"  , "Final State Particles; p_{T} [GeV]; N_{particles}", 1000, -0.5, 99.5);
	h_total_E	= new TH1F("h_total_E"	    , "Final State Particle; #sum E [GeV]; N_{evts}", 1000, -0.5, 199.5);

	//2D correlations
	h_particle_et_eta = new TH2F("h_particle_et_eta", 
			"Final State Particles; #eta; E_{T} [GeV]; N_{particles}", 
			100, -1.2, 1.2, 1000, -0.5, 99.5);
	h_particle_et_phi = new TH2F("h_particle_et_phi", 
			"Final State Particles; #varphi; E_{T} [GeV]; N_{particles}", 
			100, 0, 2*M_PI, 1000, -0.5, 99.5);
	h_particle_pt_eta = new TH2F("h_particle_pt_pta", 
			"Final State Particles; #pta; p_{T} [GeV]; N_{particles}", 
			100, -1.2, 1.2, 1000, -0.5, 99.5);
	h_particle_pt_phi = new TH2F("h_particle_pt_phi", 
			"Final State Particles; #varphi; p_{T} [GeV]; N_{particles}", 
			100, 0, 2*M_PI, 1000, -0.5, 99.5);
	h_particle_e_eta = new TH2F("h_particle_e_eta", 
			"Final State Particles; #eta; E [GeV]; N_{particles}", 
			100, -1.2, 1.2, 1000, -0.5, 99.5);
	h_particle_e_phi = new TH2F("h_particle_e_phi", 
			"Final State Particles; #varphi; E [GeV]; N_{particles}", 
			100, 0, 2*M_PI, 1000, -0.5, 99.5);
	h_particle_phi_eta = new TH2F("h_particle_phi_eta", 
			"Final State Particles; #eta; #varphi; N_{particles}", 
			100, -1.2, 1.2, 100, 0, 2*M_PI);
	
	//electrons
	h_electron_phi_eta = new TH2F("h_electron_phi_eta", 
			"Final State Particles; #eta; #varphi; N_{electrons}", 
			100, -1.2, 1.2, 100, 0, 2*M_PI);
	h_electron_pt	= new TH1F("h_electron_pt"  , "Final State Particles; p_{T} [GeV]; N_{electrons}", 1000, -0.5, 99.5);
	
	//protons
	h_proton_phi_eta = new TH2F("h_proton_phi_eta", 
			"Final State Particles; #eta; #varphi; N_{protons}", 
			100, -1.2, 1.2, 100, 0, 2*M_PI);
	h_proton_pt	= new TH1F("h_proton_pt"  , "Final State Particles; p_{T} [GeV]; N_{protons}", 1000, -0.5, 99.5);
	
	//neutrons
	h_neutron_phi_eta = new TH2F("h_neutron_phi_eta", 
			"Final State Particles; #eta; #varphi; N_{neutrons}", 
			100, -1.2, 1.2, 100, 0, 2*M_PI);
	h_neutron_pt	= new TH1F("h_neutron_pt"  , "Final State Particles; p_{T} [GeV]; N_{neutrons}", 1000, -0.5, 99.5);
	
	//pions
	h_pion_phi_eta 	= new TH2F("h_pion_phi_eta", 
			"Final State Particles; #eta; #varphi; N_{pions}", 
			100, -1.2, 1.2, 100, 0, 2*M_PI);
	h_pion_pt	= new TH1F("h_pion_pt"  , "Final State Particles; p_{T} [GeV]; N_{pions}", 1000, -0.5, 99.5);
		
	//counting events 
	h_particle_n	= new TH1I("h_particle_n", "Final state particles; N_{particle}; N_{evts}", 1000, 0, 1000);
	h_electron_n	= new TH1I("h_electron_n", "Final state electrons; N_{electron}; N_{evts}", 1000, 0, 1000);
	h_proton_n	= new TH1I("h_proton_n", "Final state protons; N_{proton}; N_{evts}", 1000, 0, 1000);
	h_neutron_n	= new TH1I("h_neutron_n", "Final state neutrons; N_{neutron}; N_{evts}", 1000, 0, 1000);
	h_pion_n	= new TH1I("h_pion_n", "Final state pions; N_{pion}; N_{evts}", 1000, 0, 1000);
	
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
	if(no_gen) return Fun4AllReturnCodes::ABORTRUN;
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
//	std::array<float,3> vertex {};
	PHHepMCGenEventMap* phg=findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
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
				HepMC::GenEvent* ev=hepev->getEvent();
				if(!ev) continue;
				else{
					auto vtx = ev->signal_process_vertex();
					if(!vtx) continue;
/*					else{ //fill in info from the generator vertex
						auto vtx_pos = vtx->position();
						vertex[0] = vtx_pos->x();
						vertex[1] = vtx_pos->y();
						vertex[2] = vtx_pos->z();
						h_vertex_x->Fill(vertex[0]);
						h_vertex_y->Fill(vertex[1]);
						h_vertex_z->Fill(vertex[2]);
						h_vertex_rz->Fill(std::sqrt(std::pow(vertex[0], 2) + std::pow(vertex[1] ,2)), vertex[3]);
						h_vertex_thetaz->Fill(std::atan2(vertex[1], vertex[0]), vertex[2]);
					}*/
					for(HepMC::GenEvent::particle_const_iterator iter=ev->particles_begin(); iter != ev->particles_end(); ++iter) 
					{
						auto particle=(*iter);
						if(!particle) continue;
						if(particle->status()==1 && !(particle->end_vertex()) )
						{
							event_particles.push_back(particle);
							if( particle->pdg_id() == 22)
							{
								photons.push_back(particle);
							}
						}
					}
				}
			}
		}
	}
	findJets(event_particles, &identified_jets);
	if(photon) 		runAnalysisPhotonJets(identified_jets, photons);
	else if (jet) 		runAnalysisJets(identified_jets);
	if( photon || jet ) 	runAnalysisEvent(event_particles);	
	return Fun4AllReturnCodes::EVENT_OK;

}
void HerwigProductionQAModule::findJets(std::vector<HepMC::GenParticle*> event_particles, std::vector<std::vector<Jet*>*>* jets)
{
	//just invoke fastJet over the relevant event particles and get some sembelance of truth jets at the HepMC level
	std::vector<fastjet::PseudoJet> candidates;
	fastjet::JetDefinition fjd2 (fastjet::antikt_algorithm, 0.2);
	fastjet::JetDefinition fjd3 (fastjet::antikt_algorithm, 0.3);
	fastjet::JetDefinition fjd4 (fastjet::antikt_algorithm, 0.4);
	fastjet::JetDefinition fjd5 (fastjet::antikt_algorithm, 0.5);
	fastjet::JetDefinition fjd6 (fastjet::antikt_algorithm, 0.6);
	for(auto p:event_particles)
	{
		auto pid = p->pdg_id();
		if (abs(pid) > 11 &&  abs(pid) < 19 ) continue;
		auto mom = p->momentum();
		candidates.push_back(fastjet::PseudoJet(mom.px(), mom.py(), mom.pz(), mom.e()));
	}	
	fastjet::ClusterSequence cs2(candidates, fjd2);
	fastjet::ClusterSequence cs3(candidates, fjd3);
	fastjet::ClusterSequence cs4(candidates, fjd4);
	fastjet::ClusterSequence cs5(candidates, fjd5);
	fastjet::ClusterSequence cs6(candidates, fjd6);
	auto js2=cs2.inclusive_jets(1.);
	auto js3=cs3.inclusive_jets(1.);
	auto js4=cs4.inclusive_jets(1.);
	auto js5=cs5.inclusive_jets(1.);
	auto js6=cs6.inclusive_jets(1.);
	std::vector< std::vector<fastjet::PseudoJet>  > jets_size
	{
		js2,
		js3,
		js4,
		js5,
		js6
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
			for(auto cmp:j.constituents())
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
	std::vector<std::vector<Jet*>*>	 jets;
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
						auto particle=(*iter);
						if(!particle) continue;
						if(particle->status()==1 && !particle->end_vertex()) //picks up final state particles only 
						{
							event_particles.push_back(particle);
							if( particle->pdg_id() == 22)
							{
								photons.push_back(particle);
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
			std::vector<Jet*> jetsize {};
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
	if(photon) 		runAnalysisPhotonJets(jets, photons);
	else if (jet)		runAnalysisJets(jets);
	if( photon || jet ) 	runAnalysisEvent(event_particles);	
	return Fun4AllReturnCodes::EVENT_OK;
}

std::vector<std::array<float, 4>> HerwigProductionQAModule::runAnalysisJets(std::vector<std::vector<Jet*>*> jets_of_all_sizes)
{
	//run analysis of jets 
	int i=0;
	std::vector<std::array<float, 4>> lead_jet_of_all_sizes {};
	for(auto jets:jets_of_all_sizes)
	{
		float lead_pt	= 0.; 
		float lead_eta	= 0.;
		float lead_phi 	= 0.;
		float lead_e	= 0.;
		int lead_comp	= 0 ;
		for(auto jet:*jets){
			float phi = jet->get_phi();
			if(phi < 0 ) phi+=2*M_PI;
			if(jet->get_pt()  > lead_pt){
			       	lead_pt 	= jet->get_pt();
				lead_eta	= jet->get_eta();
				lead_phi 	= phi;
				lead_e		= jet->get_e();
				lead_comp 	= (int)((jet->get_comp_vec()).size());
			}
			h_all_jets_pt.at(i)->	Fill(jet->get_pt());
			h_all_jets_eta.at(i)->	Fill(jet->get_eta());
			h_all_jets_phi.at(i)->	Fill(phi);
			h_all_jets_e.at(i)->	Fill(jet->get_e());
			h_all_jets_n_comp.at(i)->Fill((int)((jet->get_comp_vec()).size()));
		}
		h_lead_jets_pt.at(i)->	Fill(lead_pt);
		h_lead_jets_eta.at(i)->	Fill(lead_eta);
		h_lead_jets_phi.at(i)->	Fill(lead_phi);
		h_lead_jets_e.at(i)->	Fill(lead_e);
		h_lead_jets_n_comp.at(i)->Fill(lead_comp);
		std::array<float, 4> lead_jet {lead_pt, lead_eta, lead_phi, lead_e};
		lead_jet_of_all_sizes.push_back(lead_jet);
		i++;
		h_n_jets.at(i)->Fill((int)jets->size());
	}
	return lead_jet_of_all_sizes;
}

int HerwigProductionQAModule::runAnalysisPhotonJets(std::vector<std::vector<Jet*>*> jets_of_all_sizes, std::vector<HepMC::GenParticle*> photons)
{
	//run the analysis of the photons + jets
	auto lead_jet_of_all_sizes=runAnalysisJets(jets_of_all_sizes);
	float lead_pt	= 0.;
	float lead_eta	= 0.;
	float lead_phi 	= 0.;
	float lead_e	= 0.;

	int n_frag 	= 0;
	int n_direct	= 0;

	for(auto ph:photons)
	{
		auto p=ph->momentum();
		float pt=std::sqrt(std::pow(p.px(), 2)+ std::pow(p.py(), 2));
		float phi = p.phi();
		if(phi < 0 ) phi += 2*M_PI; 
		if(pt > lead_pt) 
		{
			lead_pt		= pt;
			lead_eta	= p.pseudoRapidity();
			lead_phi	= phi;
			lead_e		= p.e();
		}
		h_all_photons_pt->	Fill(pt);
		h_all_photons_eta->	Fill(p.pseudoRapidity());
		h_all_photons_phi->	Fill(phi);
		h_all_photons_e->	Fill(p.e());
	
		HepMC::GenVertex* prod = ph->production_vertex();
		HepMC::GenEvent* paren = ph->parent_event();
		if(*(paren->signal_process_vertex()) == *prod ){
			//this is a direct photon 
			n_direct++;
			h_direct_photons_pt->	Fill(pt);
			h_direct_photons_eta->	Fill(p.pseudoRapidity());
			h_direct_photons_phi->	Fill(phi);
			h_direct_photons_e->	Fill(p.e());
		}
		else
		{
			n_frag++;
			h_frag_photons_pt->	Fill(pt);
			h_frag_photons_eta->	Fill(p.pseudoRapidity());
			h_frag_photons_phi->	Fill(phi);
			h_frag_photons_e->	Fill(p.e());
		}

	}
	h_lead_photons_pt->Fill(lead_pt);
	h_lead_photons_eta->Fill(lead_eta);
	h_lead_photons_phi->Fill(lead_phi);
	h_lead_photons_e->Fill(lead_e);
	
	h_n_photons->Fill((int)photons.size());
	h_n_frag->Fill(n_frag);
	h_n_direct->Fill(n_direct);
	if(jet){
	//now compare correlations between lead jet and lead photon
		for(int i=0; i < (int)lead_jet_of_all_sizes.size(); i++)
		{
		       auto lead_jet = lead_jet_of_all_sizes.at(i);
		       float lead_jet_pt	= lead_jet[0];	       
		       float lead_jet_eta	= lead_jet[1];
		       float lead_jet_phi	= lead_jet[2];
		       float lead_jet_e		= lead_jet[3];
		       h_photon_jet_pt.at(i)->Fill(lead_pt, lead_jet_pt);
		       h_photon_jet_eta.at(i)->Fill(lead_eta, lead_jet_eta);
		       h_photon_jet_phi.at(i)->Fill(lead_phi, lead_jet_phi);
		       h_photon_jet_e.at(i)->Fill(lead_e, lead_jet_e);
		       float delta_phi = std::abs(lead_phi - lead_jet_phi);
		       if(delta_phi > M_PI) delta_phi = 2* M_PI - delta_phi; 
		       h_photon_jet_dphi.at(i)->Fill(delta_phi);
		}
	}
	return Fun4AllReturnCodes::EVENT_OK;
}
int HerwigProductionQAModule::runAnalysisEvent(std::vector<HepMC::GenParticle*> particles)
{
	//this is just a QA of bulk properties
	float total_E	= 0.;
	int n_p		= 0;
	int n_e 	= 0;
	int n_n 	= 0;
	int n_pi 	= 0;
	h_particle_n->Fill((int)(particles.size()));
	for(auto p:particles)
	{
		float particle_eta	= p->momentum().pseudoRapidity();
		float particle_phi	= p->momentum().phi();
		float particle_e 	= p->momentum().e();
		float particle_px 	= p->momentum().px();
		float particle_py	= p->momentum().pz();
		float particle_pt	= std::sqrt(std::pow(particle_px, 2) + std::pow(particle_py, 2));
		float particle_et	= particle_e / std::cosh(particle_eta);
		int   particle_id	= std::abs(p->pdg_id());
		
		if(particle_phi < 0 ) particle_phi += 2*M_PI;
		total_E += particle_e;
		h_particle_eta->Fill(particle_eta);
		h_particle_phi->Fill(particle_phi);
		h_particle_e->Fill(particle_e);
		h_particle_pt->Fill(particle_pt);
		h_particle_et->Fill(particle_et);
	
		h_particle_et_eta->Fill(particle_eta, particle_et);
		h_particle_et_phi->Fill(particle_phi, particle_et);
		h_particle_pt_phi->Fill(particle_phi, particle_pt);
		h_particle_pt_eta->Fill(particle_phi, particle_pt);
		h_particle_phi_eta->Fill(particle_phi, particle_eta);
		h_particle_e_phi->Fill(particle_phi, particle_e);
		h_particle_et_eta->Fill(particle_eta, particle_e);
		if(particle_id == 11 ){
		       	n_e++;
			h_electron_phi_eta->Fill(particle_phi, particle_eta);
			h_electron_pt->Fill(particle_pt);
		}
		else if(particle_id == 111 || particle_id == 211 )
		{
			n_pi++;
			h_pion_phi_eta->Fill(particle_phi, particle_eta);
			h_pion_pt->Fill(particle_pt);
		}
		else if(particle_id == 2212 )
		{
			n_p++;
			h_proton_phi_eta->Fill(particle_phi, particle_eta);
			h_proton_pt->Fill(particle_pt);
		}
		else if(particle_id == 2112)
		{
			n_n++;
			h_neutron_phi_eta->Fill(particle_phi, particle_eta);
			h_neutron_pt->Fill(particle_pt);
		}
	}
	h_electron_n->Fill(n_e);
	h_proton_n->Fill(n_p);
	h_neutron_n->Fill(n_n);
	h_pion_n->Fill(n_pi);
	h_total_E->Fill(total_E);
	return Fun4AllReturnCodes::EVENT_OK;	
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
