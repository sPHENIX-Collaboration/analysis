/*!
 *  \file		GenFitTrackProp.cc
 *  \brief		Example module to extrapolate SvtxTrack with GenFit swiming
 *  \author		Haiwang Yu <yuhw@nmsu.edu>
 */

#include "GenFitTrackProp.h"

//#include <phgenfit/Tools.h>
#include <phgenfit/Fitter.h>
#include <phgenfit/PlanarMeasurement.h>
#include <phgenfit/Track.h>
#include <phgenfit/SpacepointMeasurement.h>

#include <GenFit/RKTrackRep.h>

#include <phool/phool.h>
#include <phool/getClass.h>
#include <phgeom/PHGeomUtility.h>
#include <phfield/PHFieldUtility.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4VtxPoint.h>
#include <fun4all/PHTFileServer.h>
#include <fun4all/Fun4AllServer.h>

#include <g4hough/SvtxVertexMap.h>
#include <g4hough/SvtxVertex.h>
#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>
#include <g4hough/SvtxTrack_FastSim.h>
#include <g4hough/SvtxClusterMap.h>
#include <g4hough/SvtxCluster.h>
#include <g4hough/SvtxHitMap.h>
#include <g4hough/SvtxHit.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxTrackEval.h>
#include <g4eval/SvtxClusterEval.h>
#include <g4eval/SvtxTruthEval.h>
#include <g4eval/SvtxVertexEval.h>
#include <g4eval/SvtxHitEval.h>

#include <TTree.h>
#include <TH2D.h>
#include <TVector3.h>
#include <TDatabasePDG.h>

#include <memory>
#include <iostream>

#define LogDebug(exp)		std::cout<<"DEBUG: "	<<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogError(exp)		std::cout<<"ERROR: "	<<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"
#define LogWarning(exp)	std::cout<<"WARNING: "	<<__FILE__<<": "<<__LINE__<<": "<< exp <<"\n"

using namespace std;

GenFitTrackProp::GenFitTrackProp(const string &name, const int pid_guess) :
		SubsysReco(name),

		_fitter(nullptr),
		_track_fitting_alg_name("DafRef"),
		_do_evt_display(false),

		_pid_guess(pid_guess),

		_outfile_name("GenFitTrackProp.root"),
		_eval_tree_tracks( NULL)
{
	//initialize
	_event = 0;
}


int GenFitTrackProp::Init(PHCompositeNode *topNode) {

	cout << PHWHERE << " Openning file " << _outfile_name << endl;
	PHTFileServer::get().open(_outfile_name, "RECREATE");

	// create TTree
	_eval_tree_tracks = new TTree("T", " => tracks");

	_eval_tree_tracks->Branch("event", &event, "event/I");
	_eval_tree_tracks->Branch("gtrackID", &gtrackID, "gtrackID/I");
	_eval_tree_tracks->Branch("gflavor", &gflavor, "gflavor/I");

	_eval_tree_tracks->Branch("gpx", &gpx, "gpx/D");
	_eval_tree_tracks->Branch("gpy", &gpy, "gpy/D");
	_eval_tree_tracks->Branch("gpz", &gpz, "gpz/D");
	_eval_tree_tracks->Branch("gpt", &gpt, "gpt/D");

	_eval_tree_tracks->Branch("gvx", &gvx, "gvx/D");
	_eval_tree_tracks->Branch("gvy", &gvy, "gvy/D");
	_eval_tree_tracks->Branch("gvz", &gvz, "gvz/D");

	_eval_tree_tracks->Branch("trackID", &trackID, "trackID/I");
	_eval_tree_tracks->Branch("charge", &charge, "charge/I");
	_eval_tree_tracks->Branch("nhits", &nhits, "nhits/I");

	_eval_tree_tracks->Branch("px", &px, "px/D");
	_eval_tree_tracks->Branch("py", &py, "py/D");
	_eval_tree_tracks->Branch("pz", &pz, "pz/D");
	_eval_tree_tracks->Branch("pt", &pt, "pt/D");

	_eval_tree_tracks->Branch("dca2d", &dca2d, "dca2d/D");

	_eval_tree_tracks->Branch("radius80", &radius80, "radius80/D");
	_eval_tree_tracks->Branch("pathlength80", &pathlength80, "pathlength80/D");
	_eval_tree_tracks->Branch("pathlength85", &pathlength85, "pathlength85/D");

	return Fun4AllReturnCodes::EVENT_OK;
}

int GenFitTrackProp::End(PHCompositeNode *topNode) {

	PHTFileServer::get().cd(_outfile_name);

	_eval_tree_tracks->Write();

	//PHTFileServer::get().close();

	delete _svtxevalstack;

	delete _fitter;

	return Fun4AllReturnCodes::EVENT_OK;
}

int GenFitTrackProp::InitRun(PHCompositeNode *topNode) {

	TGeoManager* tgeo_manager = PHGeomUtility::GetTGeoManager(topNode);
  PHField * field = PHFieldUtility::GetFieldMapNode(nullptr, topNode);

	_fitter = PHGenFit::Fitter::getInstance(tgeo_manager,
	    field, _track_fitting_alg_name,
			"RKTrackRep", _do_evt_display);

	_fitter->set_verbosity(verbosity);

	if (!_fitter) {
		cerr << PHWHERE << endl;
		return Fun4AllReturnCodes::ABORTRUN;
	}

	return Fun4AllReturnCodes::EVENT_OK;
}

int GenFitTrackProp::process_event(PHCompositeNode *topNode) {
	_event++;
	if (verbosity >= 2 and _event % 1 == 0)
		cout << PHWHERE << "Events processed: " << _event << endl;

	  if (!_svtxevalstack) {
	    _svtxevalstack = new SvtxEvalStack(topNode);
	    _svtxevalstack->set_verbosity(verbosity+1);
	  } else {
	    _svtxevalstack->next_event(topNode);
	  }

	GetNodes(topNode);

	fill_tree(topNode);

	return Fun4AllReturnCodes::EVENT_OK;
}


/*!
 * Fill the trees with truth, track fit, and cluster information
 */
void GenFitTrackProp::fill_tree(PHCompositeNode *topNode) {

	// Make sure to reset all the TTree variables before trying to set them.
	reset_variables();

	event = _event;

	if (!_truth_container) {
		LogError("_truth_container not found!");
		return;
	}

	if (!_trackmap) {
		LogError("_trackmap not found!");
		return;
	}

	SvtxTrackEval* trackeval = _svtxevalstack->get_track_eval();

	for (auto track_itr = _trackmap->begin(); track_itr != _trackmap->end();
			track_itr++) {

		SvtxTrack* track = track_itr->second;

		if(!track) continue;

		trackID = track->get_id();
		charge = track->get_charge();
		nhits = track->size_clusters();

		px = track->get_px();
		py = track->get_py();
		pz = track->get_pz();

		pt = sqrt(px*px+py*py);

		dca2d = track->get_dca2d();

		auto last_state_iter = --track->end_states();

		SvtxTrackState * trackstate = last_state_iter->second;

		if(!trackstate) continue;

//		cout
//		<<__LINE__
//		<<": track->size_states(): " << track->size_states()
//		<<": track->size_clusters(): " << track->size_clusters()
//		<<endl;

		genfit::MeasuredStateOnPlane* msop80 = nullptr;

		auto pdg = unique_ptr<TDatabasePDG> (TDatabasePDG::Instance());
		int reco_charge = track->get_charge();
		int gues_charge = pdg->GetParticle(_pid_guess)->Charge();
		if(reco_charge*gues_charge<0) _pid_guess *= -1;

		genfit::AbsTrackRep* rep = new genfit::RKTrackRep(_pid_guess);

		trackstate->get_x();
		{
			TVector3 pos(trackstate->get_x(), trackstate->get_y(),
					trackstate->get_z());

			TVector3 mom(trackstate->get_px(), trackstate->get_py(),
					trackstate->get_pz());
			TMatrixDSym cov(6);
			for (int i = 0; i < 6; ++i) {
				for (int j = 0; j < 6; ++j) {
					cov[i][j] = trackstate->get_error(i, j);
				}
			}

			msop80 = new genfit::MeasuredStateOnPlane(rep);
			msop80->setPosMomCov(pos, mom, cov);

			radius80 = pos.Perp();
		}

		double radius = 85;
		TVector3 line_point(0,0,0);
		TVector3 line_direction(0,0,1);

		pathlength80 = last_state_iter->first;
		genfit::MeasuredStateOnPlane* msop85 = new genfit::MeasuredStateOnPlane(*msop80);
		rep->extrapolateToCylinder(*msop85, radius, line_point, line_direction);
		//pathlength85 = pathlength80 + rep->extrapolateToCylinder(*msop85, radius, line_point, line_direction);

		TVector3 tof_hit_pos(msop85->getPos());
		TVector3 tof_hit_norm(msop85->getPos().X(),msop85->getPos().Y(),0);
		genfit::SharedPlanePtr tof_module_plane (new genfit::DetPlane(tof_hit_pos,tof_hit_norm));

		genfit::MeasuredStateOnPlane* msop_tof_module = new genfit::MeasuredStateOnPlane(*msop80);
		pathlength85 = pathlength80 + rep->extrapolateToPlane(*msop_tof_module, tof_module_plane);

		//! Truth information
		PHG4Particle* g4particle = trackeval->max_truth_particle_by_nclusters(
				track);

		if(!g4particle) continue;

		gtrackID = g4particle->get_track_id();
		gflavor = g4particle->get_pid();

		gpx = g4particle->get_px();
		gpy = g4particle->get_py();
		gpz = g4particle->get_pz();

		gpt = sqrt(gpx*gpx+gpy*gpy);

		_eval_tree_tracks->Fill();
	}

	return;
}

/*!
 * Reset all the tree variables to their default values.
 * Needs to be called at the start of every event
 */
void GenFitTrackProp::reset_variables() {
	event = -9999;

	//-- truth
	gtrackID = -9999;
	gflavor = -9999;
	gpx = -9999;
	gpy = -9999;
	gpz = -9999;
	gvx = -9999;
	gvy = -9999;
	gvz = -9999;

	//-- reco
	trackID = -9999;
	charge = -9999;
	nhits = -9999;
	px = -9999;
	py = -9999;
	pz = -9999;
	dca2d = -9999;
}

int GenFitTrackProp::GetNodes(PHCompositeNode * topNode) {
	//DST objects
	//Truth container
	_truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode,
			"G4TruthInfo");
	if (!_truth_container && _event < 2) {
		cout << PHWHERE << " PHG4TruthInfoContainer node not found on node tree"
				<< endl;
		return Fun4AllReturnCodes::ABORTEVENT;
	}

	_trackmap = findNode::getClass<SvtxTrackMap>(topNode,
			"SvtxTrackMap");
	if (!_trackmap && _event < 2) {
		cout << PHWHERE << "SvtxTrackMap node not found on node tree"
				<< endl;
		return Fun4AllReturnCodes::ABORTEVENT;
	}

	return Fun4AllReturnCodes::EVENT_OK;
}

