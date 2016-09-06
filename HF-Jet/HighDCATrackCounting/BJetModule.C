/*!
 *  \file		BJetModule.C
 *  \brief		Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \details	Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \author		Dennis V. Perepelitsa
 */

#include "BJetModule.h"

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

#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>

#include <g4hough/SvtxVertexMap.h>
#include <g4hough/SvtxVertex.h>

#include <g4eval/SvtxEvalStack.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>

#include <HFJetTruthGeneration/HFJetDefs.h>


#define LogDebug(exp)		std::cout<<"DEBUG: "	<<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogError(exp)		std::cout<<"ERROR: "	<<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogWarning(exp)	std::cout<<"WARNING: "	<<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"


using namespace std;

BJetModule::BJetModule(const string &name) :
		SubsysReco(name),
		_verbose (true){

	_foutname = name;

}

int BJetModule::Init(PHCompositeNode *topNode) {


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

	_b_particle_n = 0;
	_b_track_n = 0;

	for (int n = 0; n < 1000; n++) {

		_b_particle_pt[n] = -1;
		_b_particle_eta[n] = -1;
		_b_particle_phi[n] = -1;
		_b_particle_pid[n] = 0;
		_b_particle_embed[n] = 0;
		_b_particle_dca[n] = -1;

		_b_track_pt[n] = -1;
		_b_track_eta[n] = -1;
		_b_track_phi[n] = -1;
		_b_track_nclusters[n] = 0;

		_b_track_dca2d[n] = -1;
		_b_track_dca2d_error[n] = -1;
		_b_track_dca2d_calc[n] = -1;
		_b_track_dca2d_calc_truth[n] = -1;
		_b_track_dca3d_calc[n] = -1;
		_b_track_dca3d_calc_truth[n] = -1;

		_b_track_dca2d_phi[n] = -99;
		_b_track_quality[n] = -99;
		_b_track_chisq[n] = -99;
		_b_track_ndf[n] = -99;

		_b_track_best_nclusters[n] = 0;
		_b_track_best_embed[n] = 0;
		_b_track_best_primary[n] = false;
		_b_track_best_pid[n] = 0;

		_b_track_best_in[n] = 0;
		_b_track_best_out[n] = 0;
		_b_track_best_parent_pid[n] = 0;

		//_b_track_particle_pt[ n ] = -1;
		//_b_track_particle_eta[ n ] = -1;
		//_b_track_particle_phi[ n ] = -1;
		//_b_track_particle_pid[ n ] = 0;

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

	_tree->Branch("particle_n", &_b_particle_n, "particle_n/I");
	_tree->Branch("particle_pt", _b_particle_pt, "particle_pt[particle_n]/F");
	_tree->Branch("particle_eta", _b_particle_eta,
			"particle_eta[particle_n]/F");
	_tree->Branch("particle_phi", _b_particle_phi,
			"particle_phi[particle_n]/F");
	_tree->Branch("particle_pid", _b_particle_pid,
			"particle_pid[particle_n]/I");
	_tree->Branch("particle_embed", _b_particle_embed,
			"particle_embed[particle_n]/i");
	_tree->Branch("particle_dca", _b_particle_dca,
			"particle_dca[particle_n]/F");

	_tree->Branch("track_n", &_b_track_n, "track_n/I");
	_tree->Branch("track_pt", _b_track_pt, "track_pt[track_n]/F");
	_tree->Branch("track_eta", _b_track_eta, "track_eta[track_n]/F");
	_tree->Branch("track_phi", _b_track_phi, "track_phi[track_n]/F");

	_tree->Branch("track_nclusters", _b_track_nclusters,
			"track_nclusters[track_n]/i");

	_tree->Branch("track_dca2d", _b_track_dca2d, "track_dca2d[track_n]/F");
	_tree->Branch("track_dca2d_error", _b_track_dca2d_error,
			"track_dca2d_error[track_n]/F");

	_tree->Branch("track_dca2d_calc", _b_track_dca2d_calc, "track_dca2d_calc[track_n]/F");
	_tree->Branch("track_dca2d_calc_truth", _b_track_dca2d_calc_truth,
			"track_dca2d_calc_truth[track_n]/F");

	_tree->Branch("track_dca3d_calc", _b_track_dca3d_calc, "track_dca3d_calc[track_n]/F");
	_tree->Branch("track_dca3d_calc_truth", _b_track_dca3d_calc_truth,
			"track_dca3d_calc_truth[track_n]/F");

	_tree->Branch("track_dca2d_phi", _b_track_dca2d_phi,
			"track_dca2d_phi[track_n]/F");

	_tree->Branch("track_quality", _b_track_quality,
			"track_quality[track_n]/F");
	_tree->Branch("track_chisq", _b_track_chisq,
			"track_chisq[track_n]/F");
	_tree->Branch("track_ndf", _b_track_ndf,
			"track_ndf[track_n]/I");

	_tree->Branch("track_best_nclusters", _b_track_best_nclusters,
			"track_best_nclusters[track_n]/i");
	_tree->Branch("track_best_embed", _b_track_best_embed,
			"track_best_embed[track_n]/i");
	_tree->Branch("track_best_primary", _b_track_best_primary,
			"track_best_primary[track_n]/O");
	_tree->Branch("track_best_pid", _b_track_best_pid,
			"track_best_pid[track_n]/I");

	_tree->Branch("track_best_in", _b_track_best_in,
			"track_best_in[track_n]/I");
	_tree->Branch("track_best_out", _b_track_best_out,
			"track_best_out[track_n]/I");
	_tree->Branch("track_best_parent_pid", _b_track_best_parent_pid,
			"track_best_parent_pid[track_n]/I");
	_tree->Branch("track_best_dca", _b_track_best_dca,
			"track_best_dca[track_n]/F");

	//_tree->Branch("track_particle_pt", _b_track_particle_pt, "track_particle_pt[track_n]/F");
	//_tree->Branch("track_particle_eta", _b_track_particle_eta, "track_particle_eta[track_n]/F");
	//_tree->Branch("track_particle_phi", _b_track_particle_phi, "track_particle_phi[track_n]/F");
	//_tree->Branch("track_particle_pid", _b_track_particle_pid, "track_particle_pid[track_n]/I");

	return 0;

}

double calc_dca(const TVector3 &track_point, const TVector3 &track_direction, const TVector3 &vertex) {

	TVector3 VP = vertex - track_point;
	double d = -99;

	if(track_direction.Mag() > 0) {
		d = (track_direction.Cross(VP)).Mag() / track_direction.Mag();
	}

	return d;
}

int BJetModule::process_event(PHCompositeNode *topNode) {

	std::cout << "DVP : at process_event #" << _ievent << std::endl;

	_b_event = _ievent;

	PHHepMCGenEvent *genevt = findNode::getClass<PHHepMCGenEvent>(topNode,
			"PHHepMCGenEvent");
	HepMC::GenEvent* theEvent = genevt->getEvent();
	//theEvent->print();

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

	PHG4TruthInfoContainer* truthinfo = findNode::getClass<
			PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

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

	PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
	//PHG4TruthInfoContainer::Range range = truthinfo->GetParticleRange();

	_b_particle_n = 0;
	LogDebug("");
	int itruth = 0;
	for (PHG4TruthInfoContainer::ConstIterator iter = range.first;
			iter != range.second; ++iter) {
		PHG4Particle* g4particle = iter->second; // You may ask yourself, why second?

		unsigned int embed_id = truthinfo->isEmbeded(
				g4particle->get_track_id());

		TLorentzVector t;
		t.SetPxPyPzE(g4particle->get_px(), g4particle->get_py(),
				g4particle->get_pz(), g4particle->get_e());

		float truth_pt = t.Pt();
		if (truth_pt < 0.5)
			continue;
		float truth_eta = t.Eta();
		if (fabs(truth_eta) > 1)
			continue;
		float truth_phi = t.Phi();
		int truth_pid = g4particle->get_pid();

		if (truth_pid == 22 || truth_pid == 2112 || truth_pid == -2112
				|| truth_pid == 130)
			continue;
		if (truth_pid == 2112 || truth_pid == -2112 || truth_pid == 130)
			continue;
		// save high-pT photons
		//if (truth_pid == 22 && truth_pt < 20) continue;
		if (truth_pid == 12 || truth_pid == -12 || truth_pid == 16
				|| truth_pid == -16 || truth_pid == 14 || truth_pid == -14)
			continue;

		_b_particle_pt[_b_particle_n] = truth_pt;
		_b_particle_eta[_b_particle_n] = truth_eta;
		_b_particle_phi[_b_particle_n] = truth_phi;
		_b_particle_pid[_b_particle_n] = truth_pid;
		_b_particle_embed[_b_particle_n] = embed_id;

		for (HepMC::GenEvent::particle_const_iterator p =
				theEvent->particles_begin(); p != theEvent->particles_end();
				++p) {

			if ((*p)->pdg_id() != truth_pid)
				continue;

			if (fabs(truth_pt - (*p)->momentum().perp()) > 0.001)
				continue;
			if (fabs(truth_eta - (*p)->momentum().pseudoRapidity()) > 0.001)
				continue;
			if (fabs(truth_phi - (*p)->momentum().phi()) > 0.001)
				continue;

			HepMC::GenVertex *production_vertex = (*p)->production_vertex();
			_b_particle_dca[_b_particle_n] =
					production_vertex->point3d().perp();
		}

		if (_verbose) {
			//std::cout << " --> truth #" << itruth << ", pt / eta / phi = " << truth_pt << " / " << truth_eta << " / " << truth_phi << ", PID " << truth_pid << ", EMBED = " << embed_id << ", parent = " << g4particle->get_parent_id() << ", primary ID = " << g4particle->get_primary_id() << std::endl;

		}

		_b_particle_n++;

		itruth++;
	}

	SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,
			"SvtxTrackMap");
	SvtxEvalStack *svtxevalstack = new SvtxEvalStack(topNode);

	SvtxVertexMap* vertexmap = findNode::getClass<SvtxVertexMap>(topNode,
			"SvtxVertexMap");

	svtxevalstack->next_event(topNode);

	SvtxTrackEval *trackeval = svtxevalstack->get_track_eval();

	int ivertex = 0;
	float vertex_x = -99;
	float vertex_y = -99;
	float vertex_z = -99;
//	float vertex_err_x = -99;
//	float vertex_err_y = -99;
	//float vertex_err_z = -99;

	for (SvtxVertexMap::Iter iter = vertexmap->begin();
			iter != vertexmap->end(); ++iter) {

		SvtxVertex* vertex = iter->second;

		//std::cout << " vertex #" << ivertex << ", x/y/z = " << vertex->get_x() << " / " << vertex->get_y() << " / " << vertex->get_z() << std::endl;

		if (ivertex == 0) {
			vertex_x = vertex->get_x();
			vertex_y = vertex->get_y();
			vertex_z = vertex->get_z();

//			vertex_err_x = sqrt(vertex->get_error(0,0));
//			vertex_err_y = sqrt(vertex->get_error(1,1));
			//vertex_err_z = sqrt(vertex->get_error(2,2));
		}

		ivertex++;

	}

	_b_track_n = 0;

	int itrack = 0;
	for (SvtxTrackMap::Iter iter = trackmap->begin(); iter != trackmap->end();
			++iter) {

		SvtxTrack* track = iter->second;

		float track_pt = track->get_pt();
		float track_eta = track->get_eta();
		float track_phi = track->get_phi();

		if (track_pt < 0.5)
			continue;
		if (fabs(track_eta) > 1)
			continue;

		std::set<PHG4Hit*> assoc_hits = trackeval->all_truth_hits(track);

		unsigned int nclusters = track->size_clusters();

		PHG4Particle* g4particle = trackeval->max_truth_particle_by_nclusters(
				track);
		unsigned int truth_nclusters = trackeval->get_nclusters_contribution(
				track, g4particle);
		unsigned int truth_embed_id = truthinfo->isEmbeded(
				g4particle->get_track_id());
		bool truth_is_primary = truthinfo->is_primary(g4particle);
		int truth_pid = g4particle->get_pid();

		//int truth_parent_id = g4particle->get_parent_id();
		//int truth_primary_id = g4particle->get_primary_id();

		//float dca = track->get_dca();
		float dca2d = track->get_dca2d();
		float dca2d_error = track->get_dca2d_error();

		TVector3 track_point(track->get_x(),track->get_y(),track->get_z());
		TVector3 track_direction(track->get_px(),track->get_py(),track->get_pz());
		TVector3 vertex(vertex_x,vertex_y,vertex_z);

		TVector3 track_point_2d(track->get_x(),track->get_y(),0);
		TVector3 track_direction_2d(track->get_px(),track->get_py(),0);
		TVector3 vertex_2d(vertex_x,vertex_y,0);

		float dca2d_calc = calc_dca(track_point_2d,track_direction_2d,vertex_2d);
		float dca2d_calc_truth = calc_dca(track_point_2d,track_direction_2d,TVector3(0,0,0));

		float dca3d_calc = calc_dca(track_point,track_direction,vertex);
		float dca3d_calc_truth = calc_dca(track_point,track_direction,TVector3(0,0,0));


//		float dca2d_calc_error = sqrt(
//				dca2d_error*dca2d_error +
//				vertex_err_x*vertex_err_x +
//				vertex_err_y*vertex_err_y);

		float x = track->get_x() - vertex_x;
		float y = track->get_y() - vertex_y;
		float z = track->get_z() - vertex_z;

		TVector3 dca_vector(x, y, z);

		float dca_phi = dca_vector.Phi();

		if (_verbose) {
			//std::cout << " --> reco-track #" << itrack << " , pt / eta / phi = " << track_pt  << " / " << track_eta << " / " << track_phi << ", nhits = " << assoc_hits.size() << ", nclusters = " << nclusters << " (of which " << truth_nclusters << " from best truth particle, which has embed = " << truth_embed_id << " and is_primary = " << truth_is_primary << " ), dca / dca2D / 2D error = " << dca << " / " << dca2d << " / " << dca2d_error << std::endl;
			//std::cout << " --> --> x / y / z = " << x << " / " << y << " / " << z << ", r = " << sqrt( x * x + y*y ) << ", rho = " << sqrt( x*x + y*y + z*z ) << std::endl;
			//std::cout << " --> --> parent / primary id = " << truth_parent_id << " / " << truth_primary_id << std::endl;
		}

		TLorentzVector t;
		t.SetPxPyPzE(g4particle->get_px(), g4particle->get_py(),
				g4particle->get_pz(), g4particle->get_e());

		float truth_pt = t.Pt();
		float truth_eta = t.Eta();
		float truth_phi = t.Phi();

		int truth_in = -1;
		int truth_out = -1;
		int truth_parent_pid = 0;
		float truth_dca = -99;

		int nhepmc = 0;
		for (HepMC::GenEvent::particle_const_iterator p =
				theEvent->particles_begin(); p != theEvent->particles_end();
				++p) {

			if ((*p)->pdg_id() != truth_pid)
				continue;

			if (fabs(truth_pt - (*p)->momentum().perp()) > 0.001)
				continue;
			if (fabs(truth_eta - (*p)->momentum().pseudoRapidity()) > 0.001)
				continue;
			if (fabs(truth_phi - (*p)->momentum().phi()) > 0.001)
				continue;

			//std::cout << " --> ===> HepMC match to pt/eta/phi = " << (*p)->momentum().perp() << " / " << (*p)->momentum().pseudoRapidity() << " / " << (*p)->momentum().phi() << std::endl << " --> ";

			//(*p)->print();

			HepMC::GenVertex *production_vertex = (*p)->production_vertex();
			//production_vertex->print();

			//std::cout << " --> " << production_vertex->particles_in_size() << " / " <<  production_vertex->particles_out_size() << " particles into / out of the vertex, which is at perp = " << production_vertex->point3d().perp() << std::endl;

			HepMC::GenVertex::particles_in_const_iterator first_parent =
					production_vertex->particles_in_const_begin();

			//std::cout << " --> first parent PDG ID is " << (*first_parent)->pdg_id() << std::endl;

			truth_in = production_vertex->particles_in_size();
			truth_out = production_vertex->particles_out_size();

			truth_parent_pid = (*first_parent)->pdg_id();

			truth_dca = production_vertex->point3d().perp();

			nhepmc++;
		}

		_b_track_pt[_b_track_n] = track_pt;
		_b_track_eta[_b_track_n] = track_eta;
		_b_track_phi[_b_track_n] = track_phi;

		_b_track_nclusters[_b_track_n] = nclusters;

		_b_track_dca2d[_b_track_n] = dca2d;
		_b_track_dca2d_error[_b_track_n] = dca2d_error;

		_b_track_dca2d_calc[_b_track_n] = dca2d_calc;
		_b_track_dca2d_calc_truth[_b_track_n] = dca2d_calc_truth;

		_b_track_dca3d_calc[_b_track_n] = dca3d_calc;
		_b_track_dca3d_calc_truth[_b_track_n] = dca3d_calc_truth;

		_b_track_dca2d_phi[_b_track_n] = dca_phi;

		_b_track_quality[_b_track_n] = track->get_quality();
		_b_track_chisq[_b_track_n] = track->get_chisq();
		_b_track_ndf[_b_track_n] = track->get_ndf();

		_b_track_best_nclusters[_b_track_n] = truth_nclusters;
		_b_track_best_embed[_b_track_n] = truth_embed_id;
		_b_track_best_primary[_b_track_n] = truth_is_primary;
		_b_track_best_pid[_b_track_n] = truth_pid;

		_b_track_best_in[_b_track_n] = truth_in;
		_b_track_best_out[_b_track_n] = truth_out;
		_b_track_best_parent_pid[_b_track_n] = truth_parent_pid;

		_b_track_best_dca[_b_track_n] = truth_dca;

		_b_track_n++;

		itrack++;

	}

	_tree->Fill();

	_ievent++;
	return 0;

}

int BJetModule::End(PHCompositeNode *topNode) {

	//_f->ls();
	//_tree->Write();
	_f->Write();
	_f->Close();

	return 0;
}

