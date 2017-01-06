/*!
 *  \file		BDiJetModule.C
 *  \brief		Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \details	Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \author		Dennis V. Perepelitsa
 */

#include "BDiJetModule.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>

#include "TLorentzVector.h"
#include <iostream>

#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>

#include <HFJetTruthGeneration/HFJetDefs.h>


#define LogDebug(exp)		std::cout<<"DEBUG: "	<<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogError(exp)		std::cout<<"ERROR: "	<<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogWarning(exp)	std::cout<<"WARNING: "	<<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"


using namespace std;

BDiJetModule::BDiJetModule(const string &name) :
		SubsysReco(name),
		_verbose (true){

	_foutname = name;

}

int BDiJetModule::Init(PHCompositeNode *topNode) {


	_ievent = 0;

	_b_event = -1;

	_b_truthjet_n = 0;

	for (int n = 0; n < 10; n++) {

		_b_truthjet_parton_flavor[n] = -9999;
		_b_truthjet_hadron_flavor[n] = -9999;

		_b_truthjet_pt[n] = -1;
		_b_truthjet_eta[n] = -1;
		_b_truthjet_phi[n] = -1;

	}


	_f = new TFile(_foutname.c_str(), "RECREATE");

	_tree = new TTree("ttree", "a withered deciduous tree");

	_tree->Branch("event", &_b_event, "event/I");

	_tree->Branch("truthjet_n", &_b_truthjet_n, "truthjet_n/I");
	_tree->Branch("truthjet_parton_flavor", _b_truthjet_parton_flavor, "truthjet_parton_flavor[truthjet_n]/I");
	_tree->Branch("truthjet_hadron_flavor", _b_truthjet_hadron_flavor, "truthjet_hadron_flavor[truthjet_n]/I");
	_tree->Branch("truthjet_pt", _b_truthjet_pt, "truthjet_pt[truthjet_n]/F");
	_tree->Branch("truthjet_eta", _b_truthjet_eta,
			"truthjet_eta[truthjet_n]/F");
	_tree->Branch("truthjet_phi", _b_truthjet_phi,
			"truthjet_phi[truthjet_n]/F");


	return 0;

}

int BDiJetModule::process_event(PHCompositeNode *topNode) {

	std::cout << "DVP : at process_event #" << _ievent << std::endl;

	_b_event = _ievent;

	// PHHepMCGenEvent *genevt = findNode::getClass<PHHepMCGenEvent>(topNode,
	// 		"PHHepMCGenEvent");
	// HepMC::GenEvent* theEvent = genevt->getEvent();
	// //theEvent->print();

	/*
	 int nhepmc = 0;
	 for ( HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin(); p != theEvent->particles_end(); ++p )
	 {

	 if ((*p)->pdg_id()  == 321 || (*p)->pdg_id()  == -321 ) {
	 std::cout << nhepmc << " : PDG ID = " << (*p)->pdg_id() << " pt = " << (*p)->momentum().perp() << std::endl;
	 (*p)->print();

	 HepMC::GenVertex *production_vertex = (*p)->production_vertex();
	 production_vertex->print();

	 std::cout << " --> " << production_vertex->particles_in_size() << " / " <<  production_vertex->particles_out_size() << " particles into / out of the vertex " << std::endl;

	 HepMC::GenVertex::particles_in_const_iterator first_parent = production_vertex->particles_in_const_begin();

	 std::cout << " --> first parent PDG ID is " << (*first_parent)->pdg_id() << std::endl;



	 }

	 nhepmc++;
	 }
	 */

	// PHG4TruthInfoContainer* truthinfo = findNode::getClass<
	// 		PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

	JetMap* truth_jets = findNode::getClass<JetMap>(topNode,
			"AntiKt_Truth_r04");
	_b_truthjet_n = 0;

	int ijet_t = 0;
	for (JetMap::Iter iter = truth_jets->begin(); iter != truth_jets->end();
			++iter) {
		Jet* this_jet = iter->second;

		float this_pt = this_jet->get_pt();
		float this_phi = this_jet->get_phi();
		float this_eta = this_jet->get_eta();

		if (this_jet->get_pt() < 10 || fabs(this_eta) > 2)
			continue;

		_b_truthjet_parton_flavor[_b_truthjet_n] = this_jet->get_property(static_cast<Jet::PROPERTY>(prop_JetPartonFlavor));
		_b_truthjet_hadron_flavor[_b_truthjet_n] = this_jet->get_property(static_cast<Jet::PROPERTY>(prop_JetHadronFlavor));
		_b_truthjet_pt[_b_truthjet_n] = this_pt;
		_b_truthjet_phi[_b_truthjet_n] = this_phi;
		_b_truthjet_eta[_b_truthjet_n] = this_eta;

		if (_verbose)
			std::cout << " truth jet #" << ijet_t << ", pt / eta / phi = "
					<< this_pt << " / " << this_eta << " / " << this_phi
					<< std::endl;

		_b_truthjet_n++;
	}

	_tree->Fill();

	_ievent++;
	return 0;

}

int BDiJetModule::End(PHCompositeNode *topNode) {

	_f->Write();
	_f->Close();

	return 0;
}

