/*!
 *  \file		BJetModule.C
 *  \brief		Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \details	Gether information from Truth Jet and Tracks, DST -> NTuple
 *  \author		Dennis V. Perepelitsa
 */

#include "BJetModule.h"

#include <HFJetTruthGeneration/HFJetDefs.h>


#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4VtxPoint.h>


#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>

#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>
#include <g4hough/SvtxClusterMap.h>
#include <g4hough/SvtxVertexMap.h>
#include <g4hough/SvtxVertex.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/JetEvalStack.h>

#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenVertex.h>

#include "TLorentzVector.h"
#include "TDatabasePDG.h"


#include <iostream>

//#define _DEBUG_

#define LogDebug(exp)		std::cout<<"DEBUG: "	<<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogError(exp)		std::cout<<"ERROR: "	<<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogWarning(exp)	std::cout<<"WARNING: "	<<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"

using namespace std;

BJetModule::BJetModule(const string &name) :
		SubsysReco(name),
		_verbose (true),
		_trackmap_name("SvtxTrackMap"),
		_vertexmap_name("SvtxVertexMap"){

	_foutname = name;

}

int BJetModule::reset_tree_vars() {

	_b_truthjet_n = 0;

	for (int n = 0; n < 10; n++) {

		_b_truthjet_parton_flavor[n] = -9999;
		_b_truthjet_hadron_flavor[n] = -9999;

		_b_truthjet_pt[n] = -99;
		_b_truthjet_eta[n] = -99;
		_b_truthjet_phi[n] = -99;

		_b_recojet_valid[n] = 0;
		_b_truthjet_pt[n] = -99;
		_b_truthjet_eta[n] = -99;
		_b_truthjet_phi[n] = -99;
	}

	_b_particle_n = 0;
	_b_track_n = 0;

	for (int n = 0; n < 1000; n++) {

		_b_particle_pt[n] = -1;
		_b_particle_eta[n] = -1;
		_b_particle_phi[n] = -1;
		_b_particle_pid[n] = 0;
		_b_particle_embed[n] = 0;

		_b_particle_vertex_x[n] = -1;
		_b_particle_vertex_y[n] = -1;
		_b_particle_vertex_z[n] = -1;

		_b_particle_dca_xy[n] = -1;
		_b_particle_dca_z[n] = -1;

		_b_track_pt[n] = -1;
		_b_track_eta[n] = -1;
		_b_track_phi[n] = -1;

		_b_track_nmaps[n] = 0;

		_b_track_nclusters[n] = 0;
		_b_track_nclusters_by_layer[n] = 0;

		_b_track_dca2d[n] = -1;
		_b_track_dca2d_error[n] = -1;
		_b_track_dca3d_xy[n] = -1;
		_b_track_dca3d_xy_error[n] = -1;
		_b_track_dca3d_z[n] = -1;
		_b_track_dca3d_z_error[n] = -1;
		_b_track_dca2d_calc[n] = -1;
		_b_track_dca2d_calc_truth[n] = -1;
		_b_track_dca3d_calc[n] = -1;
		_b_track_dca3d_calc_truth[n] = -1;

		_b_track_pca_phi[n] = -99;
		_b_track_pca_x[n] = -99;
		_b_track_pca_y[n] = -99;
		_b_track_pca_z[n] = -99;

		_b_track_quality[n] = -99;
		_b_track_chisq[n] = -99;
		_b_track_ndf[n] = -99;

		_b_track_best_nclusters[n] = 0;
		_b_track_best_nclusters_by_layer[n] = 0;
		_b_track_best_embed[n] = 0;
		_b_track_best_primary[n] = false;
		_b_track_best_pid[n] = 0;

		_b_track_best_in[n] = 0;
		_b_track_best_out[n] = 0;
		_b_track_best_parent_pid[n] = 0;

		_b_track_best_vertex_x[n] = -99;
		_b_track_best_vertex_y[n] = -99;
		_b_track_best_vertex_z[n] = -99;
		_b_track_best_dca_xy[n] = -99;
		_b_track_best_dca_z[n] = -99;

	}

	return 0;
}

int BJetModule::Init(PHCompositeNode *topNode) {
	_ievent = 0;

	_b_event = -1;

	reset_tree_vars();


	_f = new TFile(_foutname.c_str(), "RECREATE");

	_tree = new TTree("T", "a withered deciduous tree");

	_tree->Branch("event", &_b_event, "event/I");

	_tree->Branch("truth_vertex_n", &_b_truth_vertex_n, "truth_vertex_n/I");
	_tree->Branch("truth_vertex_x", _b_truth_vertex_x, "truth_vertex_x[truth_vertex_n]/F");
	_tree->Branch("truth_vertex_y", _b_truth_vertex_y, "truth_vertex_y[truth_vertex_n]/F");
	_tree->Branch("truth_vertex_z", _b_truth_vertex_z, "truth_vertex_z[truth_vertex_n]/F");

	_tree->Branch("truthjet_n", &_b_truthjet_n, "truthjet_n/I");
	_tree->Branch("truthjet_parton_flavor", _b_truthjet_parton_flavor, "truthjet_parton_flavor[truthjet_n]/I");
	_tree->Branch("truthjet_hadron_flavor", _b_truthjet_hadron_flavor, "truthjet_hadron_flavor[truthjet_n]/I");
	_tree->Branch("truthjet_pt", _b_truthjet_pt, "truthjet_pt[truthjet_n]/F");
	_tree->Branch("truthjet_eta", _b_truthjet_eta,"truthjet_eta[truthjet_n]/F");
	_tree->Branch("truthjet_phi", _b_truthjet_phi,"truthjet_phi[truthjet_n]/F");

	_tree->Branch("recojet_valid", _b_recojet_valid, "recojet_valid[truthjet_n]/I");
	_tree->Branch("recojet_pt", _b_recojet_pt, "recojet_pt[truthjet_n]/F");
	_tree->Branch("recojet_eta", _b_recojet_eta,"recojet_eta[truthjet_n]/F");
	_tree->Branch("recojet_phi", _b_recojet_phi,"recojet_phi[truthjet_n]/F");

	_tree->Branch("particle_n", &_b_particle_n, "particle_n/I");
	_tree->Branch("particle_pt", _b_particle_pt, "particle_pt[particle_n]/F");
	_tree->Branch("particle_eta", _b_particle_eta,"particle_eta[particle_n]/F");
	_tree->Branch("particle_phi", _b_particle_phi,"particle_phi[particle_n]/F");
	_tree->Branch("particle_pid", _b_particle_pid,"particle_pid[particle_n]/I");
	_tree->Branch("particle_embed", _b_particle_embed,"particle_embed[particle_n]/i");

	_tree->Branch("particle_vertex_x", _b_particle_vertex_x,"particle_vertex_x[particle_n]/F");
	_tree->Branch("particle_vertex_y", _b_particle_vertex_y,"particle_vertex_y[particle_n]/F");
	_tree->Branch("particle_vertex_z", _b_particle_vertex_z,"particle_vertex_z[particle_n]/F");
	_tree->Branch("particle_dca_xy", _b_particle_dca_xy,"particle_dca_xy[particle_n]/F");
	_tree->Branch("particle_dca_z",  _b_particle_dca_z, "particle_dca_z[particle_n]/F");

	_tree->Branch("track_n", &_b_track_n, "track_n/I");
	_tree->Branch("track_pt", _b_track_pt, "track_pt[track_n]/F");
	_tree->Branch("track_eta", _b_track_eta, "track_eta[track_n]/F");
	_tree->Branch("track_phi", _b_track_phi, "track_phi[track_n]/F");

	_tree->Branch("track_dca2d", _b_track_dca2d, "track_dca2d[track_n]/F");
	_tree->Branch("track_dca2d_error", _b_track_dca2d_error,"track_dca2d_error[track_n]/F");

	_tree->Branch("track_dca3d_xy", _b_track_dca3d_xy, "track_dca3d_xy[track_n]/F");
	_tree->Branch("track_dca3d_xy_error", _b_track_dca3d_xy_error,"track_dca3d_xy_error[track_n]/F");

	_tree->Branch("track_dca3d_z", _b_track_dca3d_z, "track_dca3d_z[track_n]/F");
	_tree->Branch("track_dca3d_z_error", _b_track_dca3d_z_error,"track_dca3d_z_error[track_n]/F");

	_tree->Branch("track_dca2d_calc", _b_track_dca2d_calc, "track_dca2d_calc[track_n]/F");
	_tree->Branch("track_dca2d_calc_truth", _b_track_dca2d_calc_truth,"track_dca2d_calc_truth[track_n]/F");

	_tree->Branch("track_dca3d_calc", _b_track_dca3d_calc, "track_dca3d_calc[track_n]/F");
	_tree->Branch("track_dca3d_calc_truth", _b_track_dca3d_calc_truth,"track_dca3d_calc_truth[track_n]/F");

	_tree->Branch("track_pca_phi", _b_track_pca_phi,"track_pca_phi[track_n]/F");
	_tree->Branch("track_pca_x", _b_track_pca_x,"track_pca_x[track_n]/F");
	_tree->Branch("track_pca_y", _b_track_pca_y,"track_pca_y[track_n]/F");
	_tree->Branch("track_pca_z", _b_track_pca_z,"track_pca_z[track_n]/F");

	_tree->Branch("track_quality", _b_track_quality,"track_quality[track_n]/F");
	_tree->Branch("track_chisq", _b_track_chisq,"track_chisq[track_n]/F");
	_tree->Branch("track_ndf", _b_track_ndf,"track_ndf[track_n]/I");

	_tree->Branch("track_nmaps", _b_track_nmaps, "track_nmaps[track_n]/I");

	_tree->Branch("track_nclusters", _b_track_nclusters,"track_nclusters[track_n]/i");
	_tree->Branch("track_nclusters_by_layer", _b_track_nclusters_by_layer,"track_nclusters_by_layer[track_n]/i");

	_tree->Branch("track_best_nclusters", _b_track_best_nclusters,"track_best_nclusters[track_n]/i");
	_tree->Branch("track_best_nclusters_by_layer", _b_track_best_nclusters_by_layer,"track_best_nclusters_by_layer[track_n]/i");

	_tree->Branch("track_best_embed", _b_track_best_embed,"track_best_embed[track_n]/i");
	_tree->Branch("track_best_primary", _b_track_best_primary,"track_best_primary[track_n]/O");
	_tree->Branch("track_best_pid", _b_track_best_pid,"track_best_pid[track_n]/I");
	_tree->Branch("track_best_pt", _b_track_best_pt,"track_best_pt[track_n]/F");
	_tree->Branch("track_best_in", _b_track_best_in,"track_best_in[track_n]/I");
	_tree->Branch("track_best_out", _b_track_best_out,"track_best_out[track_n]/I");
	_tree->Branch("track_best_parent_pid", _b_track_best_parent_pid,"track_best_parent_pid[track_n]/I");

	_tree->Branch("track_best_vertex_x", _b_track_best_vertex_x,"track_best_vertex_x[track_n]/F");
	_tree->Branch("track_best_vertex_y", _b_track_best_vertex_y,"track_best_vertex_y[track_n]/F");
	_tree->Branch("track_best_vertex_z", _b_track_best_vertex_z,"track_best_vertex_z[track_n]/F");

	_tree->Branch("track_best_dca_xy", _b_track_best_dca_xy, "track_best_dca_xy[track_n]/F");
	_tree->Branch("track_best_dca_z",  _b_track_best_dca_z, "track_best_dca_z[track_n]/F");

//	jet_eval_stack = new JetEvalStack(topNode, "AntiKt_Tower_r04", "AntiKt_Truth_r04");
//	if(!jet_eval_stack) {
//		LogError("!jet_eval_stack");
//		return Fun4AllReturnCodes::ABORTRUN;
//	}


//	svtxevalstack = new SvtxEvalStack(topNode);
//	if(!svtxevalstack) {
//		LogError("!svtxevalstack");
//		return Fun4AllReturnCodes::ABORTRUN;
//	}

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

bool calc_dca3d_line(
		double &dca_xy, double &dca_z, /*cm*/
		const TVector3 &track_point, const TVector3 &track_direction, const TVector3 &vertex) {

	dca_xy = NAN;
	dca_z = NAN;

	if(track_direction.Mag() == 0) return false;

	TVector3 PV = track_point - vertex;

	TVector3 PVxMom = track_direction.Cross(PV);

	TVector3 PCA = track_direction;

	PCA.Rotate(TMath::PiOver2(), PVxMom); // direction

	PCA.SetMag(1.);

	PCA.SetMag(PV.Dot(PCA));

	TVector3 r = track_direction.Cross(TVector3(0, 0, 1));
	r.SetMag(1.);

	dca_xy = PCA.Dot(r);

	dca_z = PCA.Z();

	return true;
}

bool calc_dca_circle_line(
		double &dca_xy, double &dca_z, /*cm*/
		const TVector3 &track_point /*cm*/, const TVector3 &track_mom /*GeV/c*/, const TVector3 &vertex, /*cm*/
		const double & q = 1./*e*/, const double & B = 1.4 /*Tesla*/) {

	double z0 = track_point.Z() - vertex.Z();
	double r0 = (track_point-vertex).Perp();
	double pz = track_mom.Z();
	double pt = track_mom.Perp();

	if(pt == 0 || B ==0 || q==0) {
		LogWarning("pt == 0 || B ==0 || q==0");
		dca_z = NAN;
		dca_xy = NAN;
		return false;
	}

	dca_z = z0 - pz/pt*r0;

	const double ten_over_c = 333.564095198; // GeV/c in cm*e*T
	double R = pt / B / q * ten_over_c; // radius in cm

	// make 2d versions
	TVector3 track_point_2d(track_point.X(),track_point.Y(),0);
	TVector3 track_mom_2d(track_mom.X(), track_mom.Y(),0);
	TVector3 vertex_2d(vertex.X(), vertex.Y(),0);

	// calc track circle center
	TVector3 track_2d_circle_center = track_mom_2d; //copy mom_2d
	track_2d_circle_center.RotateZ(TMath::PiOver2()); //rotate Pi/2
	track_2d_circle_center.SetMag(R); // scale to R, R could be negtive
	track_2d_circle_center += track_point_2d; //shift base to point_2d

	dca_xy = TMath::Abs(R) - (track_2d_circle_center-vertex_2d).Mag();

	return true;
}

int BJetModule::process_event(PHCompositeNode *topNode) {

	_b_event = _ievent;

	reset_tree_vars();

	PHHepMCGenEventMap * geneventmap = findNode::getClass<PHHepMCGenEventMap>(
			topNode, "PHHepMCGenEventMap");
	if (!geneventmap) {
		std::cout << PHWHERE
				<< " - Fatal error - missing node PHHepMCGenEventMap"
				<< std::endl;
		return Fun4AllReturnCodes::ABORTRUN;
	}

	PHHepMCGenEvent *genevt = geneventmap->get(_embedding_id);
	if (!genevt) {
		std::cout << PHWHERE
				<< " - Fatal error - node PHHepMCGenEventMap missing subevent with embedding ID "
				<< _embedding_id;
		std::cout << ". Print PHHepMCGenEventMap:";
		geneventmap->identify();
		return Fun4AllReturnCodes::ABORTRUN;
	}

	//PHHepMCGenEvent *genevt = findNode::getClass<PHHepMCGenEvent>(topNode,"PHHepMCGenEvent");
	HepMC::GenEvent* theEvent = genevt->getEvent();
	//theEvent->print();

	PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

	PHG4VtxPoint* first_point = truthinfo->GetPrimaryVtx(truthinfo->GetPrimaryVertexIndex());
	_b_truth_vertex_n = 0;
	_b_truth_vertex_x[_b_truth_vertex_n] = first_point->get_x();
	_b_truth_vertex_y[_b_truth_vertex_n] = first_point->get_y();
	_b_truth_vertex_z[_b_truth_vertex_n] = first_point->get_z();
	TVector3 truth_primary_vertex(first_point->get_x(),first_point->get_y(),first_point->get_z());
	++_b_truth_vertex_n;

	JetEvalStack *jet_eval_stack = new JetEvalStack(topNode, "AntiKt_Tower_r04", "AntiKt_Truth_r04");
	if(!jet_eval_stack) {
		LogError("!jet_eval_stack");
		return Fun4AllReturnCodes::ABORTRUN;
	}

	JetRecoEval *jet_reco_eval = jet_eval_stack->get_reco_eval();
	if(!jet_reco_eval) {
		LogError("!jet_reco_eval");
		return Fun4AllReturnCodes::ABORTRUN;
	}

	JetMap* truth_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Truth_r04");

	_b_truthjet_n = 0;
	for (JetMap::Iter iter = truth_jets->begin(); iter != truth_jets->end();
			++iter) {
		Jet* truth_jet = iter->second;

		if (truth_jet->get_pt() < 10 || fabs(truth_jet->get_eta()) > 2)
			continue;

		_b_truthjet_pt[_b_truthjet_n]  = truth_jet->get_pt();
		_b_truthjet_phi[_b_truthjet_n] = truth_jet->get_phi();
		_b_truthjet_eta[_b_truthjet_n] = truth_jet->get_eta();


		int jet_flavor = -999;

		jet_flavor = truth_jet->get_property(static_cast<Jet::PROPERTY>(prop_JetPartonFlavor));
		if(abs(jet_flavor)<100)
			_b_truthjet_parton_flavor[_b_truthjet_n] = jet_flavor;

		jet_flavor = truth_jet->get_property(static_cast<Jet::PROPERTY>(prop_JetHadronFlavor));
		if(abs(jet_flavor)<100)
			_b_truthjet_hadron_flavor[_b_truthjet_n] = jet_flavor;

		Jet* reco_jet = jet_reco_eval->best_jet_from(truth_jet);

		if(!reco_jet) continue;

		_b_recojet_valid[_b_truthjet_n] = 1;
		_b_recojet_pt[_b_truthjet_n] = reco_jet->get_pt();
		_b_recojet_phi[_b_truthjet_n] = reco_jet->get_phi();
		_b_recojet_eta[_b_truthjet_n] = reco_jet->get_eta();

		_b_truthjet_n++;
	}

	PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
	//PHG4TruthInfoContainer::Range range = truthinfo->GetParticleRange();

	_b_particle_n = 0;
	for (auto iter = range.first; iter != range.second; ++iter) {
		PHG4Particle* g4particle = iter->second; // You may ask yourself, why second?

		unsigned int embed_id = truthinfo->isEmbeded(g4particle->get_track_id());

		TLorentzVector t;
		t.SetPxPyPzE(g4particle->get_px(), g4particle->get_py(),g4particle->get_pz(), g4particle->get_e());

		float truth_pt = t.Pt();
		if (truth_pt < 0.5) continue;
		float truth_eta = t.Eta();
		if (fabs(truth_eta) > 1) continue;
		float truth_phi = t.Phi();
		int truth_pid = g4particle->get_pid();

		//original cuts
		/*
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
			*/

		//only keeps stable charged particles
		if (!(abs(truth_pid) == 211 || abs(truth_pid) == 321 || abs(truth_pid) == 2212
				|| abs(truth_pid) == 11 || abs(truth_pid) == 13))
			continue;

		_b_particle_pid[_b_particle_n] = truth_pid;
		_b_particle_pt[_b_particle_n] = truth_pt;
		_b_particle_eta[_b_particle_n] = truth_eta;
		_b_particle_phi[_b_particle_n] = truth_phi;
		_b_particle_embed[_b_particle_n] = embed_id;

		PHG4VtxPoint* point = truthinfo->GetVtx(g4particle->get_vtx_id());
		_b_particle_vertex_x[_b_particle_n] = point->get_x();
		_b_particle_vertex_y[_b_particle_n] = point->get_y();
		_b_particle_vertex_z[_b_particle_n] = point->get_z();

		TVector3 track_point(point->get_x(),point->get_y(),point->get_z());
		TVector3 track_mom(g4particle->get_px(),g4particle->get_py(),g4particle->get_pz());

		double truth_dca_xy = NAN;
		double truth_dca_z = NAN;

		//double charge = TDatabasePDG::Instance()->GetParticle(truth_pid)->Charge()/3.;
		//calc_dca_circle_line(truth_dca_xy, truth_dca_z, track_point, track_mom, truth_primary_vertex, charge, 1.4);

		calc_dca3d_line(truth_dca_xy, truth_dca_z, track_point, track_mom, truth_primary_vertex);

#ifdef _DEBUG_
		if(_verbose) {
			cout<<"track_point: --------------"<<endl;
			track_point.Print();

			cout<<"track_mom: --------------"<<endl;
			track_mom.Print();

			cout<<"truth_primary_vertex: --------------"<<endl;
			truth_primary_vertex.Print();

			cout
			<< __LINE__
			<<": " << TDatabasePDG::Instance()->GetParticle(truth_pid)->GetName()
			<<": truth_dca_xy: " << truth_dca_xy
			<<": truth_dca_z: " << truth_dca_z
			<<": charge: " << charge
			<<endl<<endl;
		}
#endif

		_b_particle_dca_xy[_b_particle_n] = truth_dca_xy;
		_b_particle_dca_z[_b_particle_n]  = truth_dca_z;


//		for (HepMC::GenEvent::particle_const_iterator p =
//				theEvent->particles_begin(); p != theEvent->particles_end();
//				++p) {
//
//			if ((*p)->pdg_id() != truth_pid)
//				continue;
//
//			if (fabs(truth_pt - (*p)->momentum().perp()) > 0.001)
//				continue;
//			if (fabs(truth_eta - (*p)->momentum().pseudoRapidity()) > 0.001)
//				continue;
//			if (fabs(truth_phi - (*p)->momentum().phi()) > 0.001)
//				continue;
//
//			HepMC::GenVertex *production_vertex = (*p)->production_vertex();
//			_b_particle_dca_xy[_b_particle_n] = production_vertex->point3d().perp();
//
//			if(_verbose) {
//				cout
//				<< __LINE__
//				<<": From HepMC: {"
//				<< production_vertex->point3d().x() <<", "
//				<< production_vertex->point3d().y() <<", "
//				<< production_vertex->point3d().z() <<"}"
//				<<endl<<endl;
//			}
//		}

		_b_particle_n++;
	}

	SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,_trackmap_name.c_str());

	SvtxVertexMap* vertexmap = findNode::getClass<SvtxVertexMap>(topNode,_vertexmap_name.c_str());

	SvtxClusterMap* clustermap = findNode::getClass<SvtxClusterMap>(topNode,"SvtxClusterMap");

	SvtxEvalStack *svtxevalstack = new SvtxEvalStack(topNode);
	if(!svtxevalstack) {
		LogError("!svtxevalstack");
		return Fun4AllReturnCodes::ABORTRUN;
	}

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
//			vertex_err_z = sqrt(vertex->get_error(2,2));
		}

		ivertex++;

	}

	_b_track_n = 0;

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

		int nmaps = 0;
		unsigned int nclusters = track->size_clusters();
		unsigned int nclusters_by_layer = 0;
		for (SvtxTrack::ConstClusterIter iter = track->begin_clusters();
				iter != track->end_clusters(); ++iter) {
			unsigned int cluster_id = *iter;
			SvtxCluster* cluster = clustermap->get(cluster_id);
			if(cluster) {
				unsigned int cluster_layer = cluster->get_layer();
				if(cluster_layer<3) ++nmaps;
				nclusters_by_layer |= (0x3FFFFFFF & (0x1 << cluster_layer));
			}
		}

		PHG4Particle* g4particle = trackeval->max_truth_particle_by_nclusters(
				track);

		unsigned int truth_nclusters = trackeval->get_nclusters_contribution(
				track, g4particle);
		unsigned int truth_nclusters_by_layer = trackeval->get_nclusters_contribution_by_layer(
				track, g4particle);

		unsigned int truth_embed_id = truthinfo->isEmbeded(
				g4particle->get_track_id());
		bool truth_is_primary = truthinfo->is_primary(g4particle);
		int truth_pid = g4particle->get_pid();

		//TVector3 g4particle_mom(g4particle->get_px(),g4particle->get_py(),g4particle->get_pz());
		//_b_track_best_pt[_b_track_n] = g4particle_mom.Pt();

		//int truth_parent_id = g4particle->get_parent_id();
		//int truth_primary_id = g4particle->get_primary_id();

		//float dca = track->get_dca();
		float dca2d = track->get_dca2d();
		float dca2d_error = track->get_dca2d_error();

		float dca3d_xy = track->get_dca3d_xy();
		float dca3d_xy_error = track->get_dca3d_xy_error();

		float dca3d_z = track->get_dca3d_z();
		float dca3d_z_error = track->get_dca3d_z_error();

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

		_b_track_pca_x[_b_track_n] = track->get_x() - vertex_x;
		_b_track_pca_y[_b_track_n] = track->get_y() - vertex_y;
		_b_track_pca_z[_b_track_n] = track->get_z() - vertex_z;

		TVector3 dca_vector(
				_b_track_pca_x[_b_track_n],
				_b_track_pca_y[_b_track_n],
				_b_track_pca_z[_b_track_n]);

		_b_track_pca_phi[_b_track_n] = dca_vector.Phi();

		TLorentzVector t;
		t.SetPxPyPzE(g4particle->get_px(), g4particle->get_py(),
				g4particle->get_pz(), g4particle->get_e());

		float truth_pt = t.Pt();
		float truth_eta = t.Eta();
		float truth_phi = t.Phi();

		PHG4VtxPoint* point = truthinfo->GetVtx(g4particle->get_vtx_id());
		_b_track_best_vertex_x[_b_track_n] = point->get_x();
		_b_track_best_vertex_y[_b_track_n] = point->get_y();
		_b_track_best_vertex_z[_b_track_n] = point->get_z();


		TVector3 track_best_point(point->get_x(),point->get_y(),point->get_z());
		TVector3 track_best_mom(g4particle->get_px(),g4particle->get_py(),g4particle->get_pz());

		double truth_dca_xy = NAN;
		double truth_dca_z = NAN;

		//double charge = TDatabasePDG::Instance()->GetParticle(truth_pid)->Charge()/3.;
		//calc_dca_circle_line(truth_dca_xy, truth_dca_z, track_best_point, track_best_mom, truth_primary_vertex, charge, 1.4);

		calc_dca3d_line(truth_dca_xy, truth_dca_z, track_best_point, track_best_mom, truth_primary_vertex);

		_b_track_best_dca_xy[_b_track_n] = truth_dca_xy;
		_b_track_best_dca_z[_b_track_n] = truth_dca_z;

		//_b_track_best_parent_pid[_b_track_n] = truthinfo->GetParticle(g4particle->get_parent_id())->get_pid();
		int truth_in = -1;
		int truth_out = -1;
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

			HepMC::GenVertex *production_vertex = (*p)->production_vertex();


			truth_in = production_vertex->particles_in_size();
			truth_out = production_vertex->particles_out_size();

			HepMC::GenVertex::particles_in_const_iterator first_parent =
					production_vertex->particles_in_const_begin();
			_b_track_best_parent_pid[_b_track_n] = (*first_parent)->pdg_id();

			nhepmc++;
		}

		_b_track_pt[_b_track_n] = track_pt;
		_b_track_eta[_b_track_n] = track_eta;
		_b_track_phi[_b_track_n] = track_phi;

		_b_track_dca2d[_b_track_n] = dca2d;
		_b_track_dca2d_error[_b_track_n] = dca2d_error;

		_b_track_dca3d_xy[_b_track_n] = dca3d_xy;
		_b_track_dca3d_xy_error[_b_track_n] = dca3d_xy_error;

		_b_track_dca3d_z[_b_track_n] = dca3d_z;
		_b_track_dca3d_z_error[_b_track_n] = dca3d_z_error;

		_b_track_dca2d_calc[_b_track_n] = dca2d_calc;
		_b_track_dca2d_calc_truth[_b_track_n] = dca2d_calc_truth;

		_b_track_dca3d_calc[_b_track_n] = dca3d_calc;
		_b_track_dca3d_calc_truth[_b_track_n] = dca3d_calc_truth;

//		_b_track_pca_phi[_b_track_n] = dca_phi;
//		_b_track_pca_x[_b_track_n] = dca_x;
//		_b_track_pca_y[_b_track_n] = dca_y;
//		_b_track_pca_z[_b_track_n] = dca_z;

		_b_track_quality[_b_track_n] = track->get_quality();
		_b_track_chisq[_b_track_n] = track->get_chisq();
		_b_track_ndf[_b_track_n] = track->get_ndf();

		_b_track_nmaps[_b_track_n] = nmaps;

		_b_track_nclusters[_b_track_n] = nclusters;
		_b_track_nclusters_by_layer[_b_track_n] = nclusters_by_layer;

		_b_track_best_nclusters[_b_track_n] = truth_nclusters;
		_b_track_best_nclusters_by_layer[_b_track_n] = truth_nclusters_by_layer;
		_b_track_best_embed[_b_track_n] = truth_embed_id;
		_b_track_best_primary[_b_track_n] = truth_is_primary;
		_b_track_best_pid[_b_track_n] = truth_pid;
		_b_track_best_pt[_b_track_n] = truth_pt;

		_b_track_best_in[_b_track_n] = truth_in;
		_b_track_best_out[_b_track_n] = truth_out;

		_b_track_n++;
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

//	delete jet_eval_stack;
//	delete svtxevalstack;

	return 0;
}

